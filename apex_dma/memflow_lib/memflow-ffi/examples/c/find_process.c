#include "memflow.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

ProcessInstance target_process;
ProcessInstance target_process2;

struct FindProcessContext {
  OsInstance *os;
  const char *name;
  ProcessInstance *target_process;
  bool found;
};

bool find_process(struct FindProcessContext *find_context, Address addr) {

  if (find_context->found) {
    return false;
  }

  if (mf_osinstance_process_by_address(find_context->os, addr,
                                       find_context->target_process)) {
    return true;
  }

  const struct ProcessInfo *info =
      mf_processinstance_info(find_context->target_process);

  if (!strcmp(info->name, find_context->name)) {
    // abort iteration
    find_context->found = true;
    return false;
  }

  mf_processinstance_drop(*find_context->target_process);

  // continue iteration
  return true;
}

pthread_mutex_t lock;

void *readmem(void *vargp) {
  // Store the value argument passed to this thread
  int *myid = (int *)vargp;

  uint8_t buffer[0x8];

  while (true) {
    pthread_mutex_lock(&lock);
    mf_processinstance_read_raw_into(&target_process, 0x7FF72AF30000,
                                     MUT_SLICE(u8, buffer, sizeof(buffer)));

    printf("TID: %d, Read: %lx\n", *myid, *(uint64_t *)buffer);
    pthread_mutex_unlock(&lock);
  }
  return 0;
}

int main(int argc, char *argv[]) {

  int ret = 0;

  // enable info level logging
  log_init(3);

  // load all available plugins
  Inventory *inventory = inventory_scan();
  printf("inventory initialized: %p\n", inventory);

  const char *conn_name = argc > 1 ? argv[1] : "kvm";
  const char *conn_arg = argc > 2 ? argv[2] : "";
  const char *os_name = argc > 3 ? argv[3] : "win32";
  const char *os_arg = argc > 4 ? argv[4] : "";
  const char *target_proc = argc > 5 ? argv[5] : "notepad.exe";

  ConnectorInstance connector, *conn = conn_name[0] ? &connector : NULL;

  // initialize the connector plugin
  if (conn) {
    if (inventory_create_connector(inventory, conn_name, conn_arg, conn)) {
      log_error("unable to initialize connector");
      inventory_free(inventory);
      return 1;
    }

    printf("connector initialized: %p\n",
           connector.container.instance.instance);
  }

  // initialize the OS plugin
  OsInstance os;
  if (inventory_create_os(inventory, os_name, os_arg, conn, &os)) {
    log_error("unable to initialize os plugin");
    inventory_free(inventory);
    return 1;
  }

  printf("os plugin initialized: %p\n", os.container.instance.instance);

  // find a specific process based on it's name.
  // this can easily be replaced by process_by_name but
  // is being used here as a demonstration.

  /* struct FindProcessContext find_context = {
       &os,
       target_proc,
       &target_process,
       false,
   };

   mf_osinstance_process_address_list_callback(
       &os, CALLBACK(Address, &find_context, find_process));

   if (find_context.found) {
     const struct ProcessInfo *info = mf_processinstance_info(&target_process);

     printf("%s process found: 0x%lx] %d %s %s\n", target_proc, info->address,
            info->pid, info->name, info->path);

     mf_processinstance_drop(target_process);
   } else {
     printf("Unable to find %s\n", target_proc);
   }*/

  // find a specific process based on its name
  // via process_by_name
  if (!(ret = mf_osinstance_process_by_name(&os, STR(target_proc),
                                            &target_process))) {
    const struct ProcessInfo *info = mf_processinstance_info(&target_process);

    printf("%s process found: 0x%lx] %d %s %s\n", target_proc, info->address,
           info->pid, info->name, info->path);

    ModuleInfo module_info;
    mf_processinstance_module_by_name(&target_process, STR(target_proc),
                                      &module_info);
    printf("0x%lx\n", module_info.base);

    /*ConnectorInstance conn2;
    if (inventory_create_connector(inventory, conn_name, conn_arg, &conn2)) {
  log_error("unable to initialize connector");
  inventory_free(inventory);
  return 1;
}

ConnectorInstance cloned = mf_connectorinstance_clone(&conn2);
    printf("ok1\n");

mf_connectorinstance_drop(cloned);
    printf("ok2\n");*/

    // MemoryView phys_view = mf_connectorinstance_phys_view(&conn2);

    /*if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

    pthread_t tid;
    pthread_t tid2;
    pthread_create(&tid, NULL, readmem, (void *)&tid);
    pthread_create(&tid2, NULL, readmem, (void *)&tid2);

    pthread_join(tid, NULL);
    pthread_join(tid2, NULL);

     pthread_mutex_destroy(&lock);*/

    if (!(ret = mf_osinstance_process_by_name(&os, STR("notepad++.exe"),
                                              &target_process2))) {
      const struct ProcessInfo *info2 =
          mf_processinstance_info(&target_process2);

      printf("%s process found: 0x%lx] %d %s %s\n", "notepad++.exe",
             info2->address, info2->pid, info2->name, info2->path);

      uint8_t buffer[0x8];

      mf_processinstance_read_raw_into(&target_process, info->address + 0x520,
                                       MUT_SLICE(u8, buffer, sizeof(buffer)));

      printf("Read: %lx\n", *(uint64_t *)buffer);

      mf_processinstance_read_raw_into(&target_process2, info2->address + 0x520,
                                       MUT_SLICE(u8, buffer, sizeof(buffer)));

      printf("Read2: %lx\n", *(uint64_t *)buffer);
    } else {
      printf("error notepad++");
    }

    mf_processinstance_drop(target_process);
  } else {
    printf("Unable to find %s\n", target_proc);
    log_debug_errorcode(ret);
  }

  // This will also free the connector here
  // as it was _moved_ into the os by `inventory_create_os`
  mf_osinstance_drop(os);
  log_info("os plugin/connector freed");

  inventory_free(inventory);
  log_info("inventory freed");

  return 0;
}
