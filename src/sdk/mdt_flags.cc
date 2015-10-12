#include <gflags/gflags.h>

// nfs cluster location
DEFINE_string(env_fs_type, "dfs", "file system type");
DEFINE_string(env_nfs_mountpoint, "/disk/tera", "nfs mount point");
DEFINE_string(env_nfs_conf_path, "./conf/nfs.conf", "nfs's client configure file");
DEFINE_string(env_nfs_so_path, "./lib/libnfs.so", "nfs's client's .so lib");

// tera cluster location
DEFINE_string(tera_flag_file_path, "./conf/tera.flag", "tera's client's configure file");
DEFINE_string(tera_root_dir, "/disk/tera/trace-sys/", "tera cluster's dir");

// mdt cluster
DEFINE_string(database_root_dir, "/disk/tera/DatabaseDir", "database's data file dir");

// write ops param
DEFINE_int64(concurrent_write_handle_num, 10, "num of fs writer");
DEFINE_int64(max_write_handle_seq, 10, "max num of req can schedule to current write_handle");
DEFINE_int64(data_size_per_sync, 0, "num of data per Sync()");
