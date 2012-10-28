/* makeat64: a tool for creating a T64 from other T64, PRG or P00 files.
 *
 * Copyright (c) Fabrizio Gennari, 2006
 *
 * Cut'n'pasted from bits and pieces of WAV-PRG
 *
 * The program is distributed under the GNU General Public License.
 * See file LICENSE.TXT for details.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "create_t64.h"
#include "t64utils.h"
#include "process_input_files.h"
#include "wav2prg_block_list.h"
#include "block_list.h"

struct entry_element{
  int entry;
  struct entry_element* next;
};

void help(const char* progname){
  printf("Usage: %s -h|-V\n", progname);
  printf("       %s [-u] [-n <t64 name>] <output_T64_file_name> <file> [file...]\n", progname);
  printf("       %s -l <file> [file...]\n", progname);
  printf("Options:\n");
  printf("\t-h: show this help message and exit successfully\n");
  printf("\t-V: show version and copyright info and exit successfully\n");
  printf("\t-u use file name as C64 name for PRG files\n");
  printf("\t-n <t64 name> use <t64 name> as T64 internal name\n");
  printf("\t-l don't create a new file, list content of arguments\n");
}

void version(){
  printf("makeat64 version 1.0\n");
  printf("A tool created by cutting and pasting other stuff\n\n");
  printf("(C) by Fabrizio Gennari, 2006\n");
  printf("This program is distributed under the GNU General Public License\n");
  printf("Read the file LICENSE.TXT for details\n");
  printf("This product includes software developed by the NetBSD\n");
  printf("Foundation, Inc. and its contributors\n");
}

int main(int numarg,char** argo){
  int show_help=0;
  int show_version=0;
  int option;
  char *progname=argo[0];
  char *outname;
  unsigned char use_filename_as_c64_name=0;
  char *t64_name=NULL;
  int list_asked=0;
  struct simple_block_list_element *blocks = NULL;
  uint8_t use_whole_t64 = 1;
  struct block_list_element* outblocks; 
  struct block_list_element** outblock = &outblocks;
  /* Get options */

  while ((option=getopt(numarg,argo,"hVun:la"))!=EOF){
    switch(option){
    case 'h':
      show_help=1;
      break;
    case 'V':
      show_version=1;
      break;
    case 'u':
      use_filename_as_c64_name=1;
      break;
    case 'n':
      t64_name=optarg;
      break;
    case 'l':
      list_asked=1;
      break;
    case 'a':
      use_whole_t64=0;
      break;
    default:
      help(progname);
      return 1;
    }
  }
  if (show_help==1){
    help(progname);
    return 0;
  }
  if (show_version==1){
    version();
    return 0;
  }
  argo += optind;
  numarg -= optind;

  if (!list_asked){
    outname = argo[0];
    numarg -= 1;
    argo += 1;
  }
  if (numarg < 1){
    printf("Not enough arguments!%u\n",numarg);
    help(progname);
    return 1;
  }
  
  blocks = process_input_files(numarg, argo, list_asked, use_filename_as_c64_name, use_whole_t64);
  
  if(!list_asked)
  {
    while(blocks != NULL){
      *outblock = (struct block_list_element*)calloc(1, sizeof(struct block_list_element));
      memcpy(&(*outblock)->block, &blocks->block, sizeof(struct program_block));
      (*outblock)->real_start = blocks->block.info.start;
      (*outblock)->real_end = blocks->block.info.end;
      outblock = &(*outblock)->next;
      blocks = blocks->next;
    }
    create_t64(outblocks, t64_name, outname, wav2prg_true);
  }

  return 0;
}

