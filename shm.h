#ifndef MEW_SHM_H
#define MEW_SHM_H

#include <semaphore.h>
#include <stdint.h>
#include <stdbool.h>

struct mew_shm_data {
	sem_t commit_ready;
	sem_t display_done;

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
void shm_destroy(struct mew_shm *shm);
bool shm_display_done(struct mew_shm *shm);
void shm_commit(struct mew_shm *shm, uint32_t committed);

#endif
