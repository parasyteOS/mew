#ifndef MEW_SHM_H
#define MEW_SHM_H

#include <stdint.h>
#include <stdbool.h>

#define SHM_STATUS_COMMIT_READY (1U << 0)
#define SHM_STATUS_CLIENT_DONE  (1U << 1)

struct mew_shm_data {
	uint32_t size;

	uint32_t status;

	uint32_t committed;
	uint32_t width;
	uint32_t height;

	uint32_t format;
	uint32_t stride;
	char pixels[];
};

struct mew_shm {
	int fd;
	struct mew_shm_data *data;
};

struct mew_shm *shm_create(const char *backfile, uint32_t format, uint32_t width, uint32_t height);
bool shm_set_rect(struct mew_shm *shm, uint32_t width, uint32_t height);
bool shm_status_check(struct mew_shm *shm, uint32_t status);
void shm_status_set(struct mew_shm *shm, uint32_t status);
void shm_status_clear(struct mew_shm *shm, uint32_t status);
void shm_commit(struct mew_shm *shm, uint32_t committed);
void shm_destroy(struct mew_shm *shm);

#endif
