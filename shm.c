#define _GNU_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>

#include <wlr/util/log.h>
#include <wlr/types/wlr_output.h>

#include "shm.h"

struct mew_shm *shm_create(const char *backfile, uint32_t format, uint32_t width, uint32_t height)
{
	int fd;
	bool init = false;
	struct mew_shm *shm;
	struct mew_shm_data *data;
	// FIXME: hardcode
	uint32_t stride = width * 4;
	uint32_t size = sizeof(*data) + sizeof(*(data->pixels)) * stride * height;

	if (width <= 0 || height <= 0) {
		wlr_log(WLR_ERROR, "Invalid state: width: %d, height: %d", width, height);
		return NULL;
	}

	shm = calloc(1, sizeof(*shm));
	if (!shm) {
		wlr_log(WLR_ERROR, "Failed to allocate memory for shm");
		return NULL;
	}

	if (access(backfile, F_OK)) {
		if (errno != ENOENT) {
			wlr_log(WLR_ERROR, "Failed to access %s: %s", backfile, strerror(errno));
			return NULL;
		}
		init = true;
	}

	fd = open(backfile, O_CLOEXEC|O_CREAT|O_RDWR, 0600);
	if (fd < 0) {
		wlr_log(WLR_ERROR, "Failed to open shm backfile %s", backfile);
		goto free;
	}

	if (ftruncate(fd, size)) {
		wlr_log(WLR_ERROR, "Failed to ftruncate: %s", strerror(errno));
		goto close;
	}

	data = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		wlr_log(WLR_ERROR, "Failed to mmap data: %s", strerror(errno));
		goto close;
	}

	if (init) {
		if (sem_init(&data->commit_ready, 1, 0)) {
			wlr_log(WLR_ERROR, "Failed to init semaphore commit_ready: %s", strerror(errno));
			goto unmap;
		}

		if (sem_init(&data->display_done, 1, 0)) {
			wlr_log(WLR_ERROR, "Failed to init semaphore display_done: %s", strerror(errno));
			goto unmap;
		}
	}

	data->committed = WLR_OUTPUT_STATE_MODE;
	data->width = width;
	data->height = height;
	data->format = format;
	data->stride = stride;

	if (sem_post(&data->commit_ready)) {
		wlr_log(WLR_ERROR, "Failed to post commit_ready: %s", strerror(errno));
	}

	shm->fd = fd;
	shm->data = data;

	return shm;

unmap:
	munmap(data, size);
close:
	close(fd);
free:
	free(shm);
	if (init)
		unlink(backfile);
	return NULL;
}

bool shm_set_rect(struct mew_shm *shm, uint32_t width, uint32_t height)
{
	int fd = shm->fd;
	struct mew_shm_data *data = shm->data;
	uint32_t stride = width * 4;
	uint32_t size = sizeof(*data) + sizeof(*(data->pixels)) * stride * height;

	if (ftruncate(fd, size)) {
		wlr_log(WLR_ERROR, "Failed to ftruncate: %s", strerror(errno));
		return false;
	}

	data = mremap(data, 0, size, MREMAP_MAYMOVE);
	if (data == MAP_FAILED) {
		wlr_log(WLR_ERROR, "Failed to remap shm: %s", strerror(errno));
		return false;
	}

	data->width = width;
	data->height = height;
	data->stride = stride;

	shm->data = data;
	return true;
}

void shm_destroy(struct mew_shm *shm)
{
	int fd = shm->fd;
	struct mew_shm_data *data = shm->data;
	int32_t height = data->height;
	uint32_t stride = data->stride;
	uint32_t size = sizeof(*data) + sizeof(*(data->pixels)) * stride * height;
	munmap(data, size);
	close(fd);
}

void shm_commit(struct mew_shm *shm, uint32_t committed)
{
	shm->data->committed = committed;
	sem_post(&shm->data->commit_ready);
}

bool shm_display_done(struct mew_shm *shm)
{
	if (sem_trywait(&shm->data->display_done)) {
		if (errno == EAGAIN)
			wlr_log(WLR_DEBUG, "Client not ready, skip shm update");
		else
			wlr_log(WLR_ERROR, "sem_trywait: %s", strerror(errno));

		return false;
	}

	return true;
}
