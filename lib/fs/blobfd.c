#include<libtransistor/fs/blobfd.h>
#include<libtransistor/fd.h>
#include<errno.h>
#include<stdio.h>
#include<string.h>

static off_t blobfd_llseek(void *vfile, off_t offset, int whence) {
	blob_file *file = vfile;
	switch(whence) {
	case SEEK_SET:
		file->head = offset;
		break;
	case SEEK_CUR:
		file->head+= offset;
		break;
	case SEEK_END:
		file->head = file->size + offset;
		break;
	default:
		errno = EINVAL;
		return -1;
	}
	return file->head;
}

static ssize_t blobfd_read(void *vfile, char *buffer, size_t size) {
	blob_file *file = vfile;
	size_t sz = size;
	if(file->head + sz > file->size) {
		sz = file->size - file->head;
	}
	memcpy(buffer, file->data + file->head, sz);
	file->head+= sz;
	return sz;
}

static ssize_t blobfd_write(void *vfile, const char *buffer, size_t size) {
	return -EROFS;
}

static int blobfd_flush(void *vfile) {
	return 0;
}

struct file;
static int blobfd_release(struct file *f) {
	return 0;
}

static struct file_operations blobfd_ops = {
	.llseek = blobfd_llseek,
	.read = blobfd_read,
	.write = blobfd_write,
	.flush = blobfd_flush,
	.release = blobfd_release
};

int blobfd_create(blob_file *file, void *blob, size_t size) {
	file->data = blob;
	file->size = size;
	file->head = 0;
	return fd_create_file(&blobfd_ops, file);
}
