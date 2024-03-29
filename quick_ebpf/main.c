#include <stdio.h>
#include <stdlib.h>
#include <bpf/libbpf.h>
#include "build/ebpf_programs_autogen.h"

static int my_libbpf_print(enum libbpf_print_level level, const char *format, va_list args)
{
  return vfprintf(stderr, format, args);
}

int main(int argc, char **argv)
{
  int err = 0;

  libbpf_set_strict_mode(LIBBPF_STRICT_ALL);
  libbpf_set_print(my_libbpf_print);

  for (int i = 0; i < sizeof(ebpf_programs) / sizeof(ebpf_programs[0]); i++)
  {
    struct bpf_program_data *pdata = &ebpf_programs[i];
    pdata->skel = pdata->skeleton_open_and_load();

    if (!pdata->skel)
    {
      fprintf(stderr, "Failed to open and load BPF skeleton for %s\n", pdata->name);
      err = -1;
      goto cleanup;
    }

    if (pdata->skeleton_attach(pdata->skel))
    {
      fprintf(stderr, "Failed to attach BPF skeleton for %s\n", pdata->name);
      pdata->skeleton_destroy(pdata->skel);
      pdata->skel = NULL;
      err = -1;
      goto cleanup;
    }

    printf("%s eBPF program loaded and attached successfully\n", pdata->name);
  }

  // TODO: Add application logic here like waiting for signals ctrl+c, loop, etc.

cleanup:
  for (int i = 0; i < sizeof(ebpf_programs) / sizeof(ebpf_programs[0]); i++)
  {
    if (ebpf_programs[i].skel)
    {
      ebpf_programs[i].skeleton_destroy(ebpf_programs[i].skel);
      printf("%s eBPF program cleaned up\n", ebpf_programs[i].name);
    }
  }

  return err;
}
