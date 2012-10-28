/*----------------------------------------------------------------------

  FIXCONV - a program which tries to fix bad T64 files
  (like the ones created by Conv64)
  by Fabrizio Gennari, (C) 2000-2006
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  --------------------------------------------------------------------*/
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <errno.h>

#include "t64utils.h"
#include "program_block.h"

#ifndef __GNU_LIBRARY__
#define strcasecmp _stricmp
#endif

long file_size(FILE* descriptor){
  long result;
  fseek(descriptor, 0, SEEK_END);
  result=ftell(descriptor);
  fseek(descriptor, 0, SEEK_SET);
  return result;
}

int lastpart(const char* total, const char* last){
  int lendiff;
  lendiff=strlen(total)-strlen(last);
  if (lendiff<1) return 0;
  if (strcasecmp(total+lendiff,last)) return 0;
  return 1;
}

int main(int argc, char** argv){
  char *tapename;
  int currarg,used_entries,total_entries,filesize,offset,diff1,diff2,entries,entry,i;
  unsigned char endadd_low, endadd_high;
  FILE* desc;
  struct program_block program;

  for (currarg=1;currarg<argc;currarg++){
    if (!lastpart(argv[currarg],".t64")){
      tapename=(char*)malloc(strlen(argv[currarg])+5);
      sprintf(tapename,"%s.t64",argv[currarg]);
    }
    else{
      tapename=(char*)malloc(strlen(argv[currarg])+1);
      sprintf(tapename,"%s",argv[currarg]);
    }
    printf("%s: ",tapename);

    desc=fopen(tapename,"rb");
    if (desc==NULL){
       printf("Cannot open file\n",tapename);
       goto end2;
    }
    if (detect_type(desc)!=t64){
       printf("The file is not a T64\n");
       goto end;
    }
    if ((entries=get_used_entries(desc))!=1){
       printf("Sorry, has %d entries, not 1\n",entries);
       goto end;
    }
    total_entries=get_total_entries(desc);
    for(i=1;i<total_entries;i++)
      if (get_entry_info(i,desc,&program,&offset))
        break;
    printf("1 entry, name %s, ",program.info.name);
    diff1=program.info.end-program.info.start;
    filesize=file_size(desc);
    diff2=filesize-offset;
    if (diff1==diff2){
       printf("File OK\n");
       goto end;
    }
    printf("broken, fixing...");
    program.info.end=program.info.start+diff2;
    endadd_low=program.info.end&255;
    endadd_high=program.info.end>>8;
    fclose(desc);

    desc=fopen(tapename,"r+b");
    if (desc==NULL){
      printf("failed: %s\n", strerror(errno));
      goto end2;
    }
    fseek(desc,36+32*entry,SEEK_SET);
    fwrite(&endadd_low ,1,1,desc);
    fwrite(&endadd_high,1,1,desc);
    printf("Fixed\n");
end:
    fclose(desc);
end2:
    free(tapename);
  }
  return 0;
}
