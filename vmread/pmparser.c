#include "pmparser.h"

procmaps_struct* g_last_head = NULL;
procmaps_struct* g_current = NULL;

void _pmparser_split_line(
		char*buf,char*addr1,char*addr2,
		char*perm,char* offset,char* device,char*inode,
		char* pathname){
	int orig=0;
	int i=0;
	while(buf[i]!='-'){
		addr1[i-orig]=buf[i];
		i++;
	}
	addr1[i]='\0';
	i++;

	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		addr2[i-orig]=buf[i];
		i++;
	}
	addr2[i-orig]='\0';


	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		perm[i-orig]=buf[i];
		i++;
	}
	perm[i-orig]='\0';

	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		offset[i-orig]=buf[i];
		i++;
	}
	offset[i-orig]='\0';

	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		device[i-orig]=buf[i];
		i++;
	}
	device[i-orig]='\0';

	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i]!='\t' && buf[i]!=' '){
		inode[i-orig]=buf[i];
		i++;
	}
	inode[i-orig] = '\0';

	pathname[0] = '\0';
	while(buf[i]=='\t' || buf[i]==' ')
		i++;
	orig=i;
	while(buf[i] != '\t' && buf[i] != ' ' && buf[i] != '\n'){
		pathname[i-orig]=buf[i];
		i++;
	}
	pathname[i-orig] = '\0';

}

/**
 * pmparser_parse
 * @param pid the process id whose memory map to be parser. the current process if pid<0
 * @return list of procmaps_struct structers
 */
procmaps_struct* pmparser_parse(int pid){
	char maps_path[500];
	if(pid >= 0)
		sprintf(maps_path,"/proc/%d/maps",pid);
    else
		sprintf(maps_path,"/proc/self/maps");
	FILE* file = fopen(maps_path,"r");
	if(!file)
		return NULL;
	int ind=0;char buf[3000];
	char c;
	procmaps_struct* list_maps=NULL;
	procmaps_struct* tmp;
	procmaps_struct* current_node=list_maps;
	char addr1[20],addr2[20], perm[8], offset[20], dev[10],inode[30],pathname[600];
	while(1){
		if( (c=(char)fgetc(file))==EOF ) break;
		fgets(buf+1,259,file);
		buf[0]=c;
		tmp= (procmaps_struct*)malloc(sizeof(procmaps_struct));
		_pmparser_split_line(buf,addr1,addr2,perm,offset, dev,inode,pathname);

		sscanf(addr1,"%lx",(long unsigned *)&tmp->addr_start);
		sscanf(addr2,"%lx",(long unsigned *)&tmp->addr_end);
		tmp->length = ((unsigned long)tmp->addr_end - (unsigned long)tmp->addr_start);
		strcpy(tmp->perm,perm);
		tmp->is_r = (perm[0]=='r');
		tmp->is_w = (perm[1]=='w');
		tmp->is_x = (perm[2]=='x');
		tmp->is_p = (perm[3]=='p');

		sscanf(offset,"%lx",(unsigned long*)&tmp->offset);

		strcpy(tmp->dev,dev);

		tmp->inode=atoi(inode);

		strcpy(tmp->pathname,pathname);
		tmp->next=NULL;

		if(ind==0){
			list_maps=tmp;
			list_maps->next=NULL;
			current_node=list_maps;
		}
		current_node->next=tmp;
		current_node=tmp;
		ind++;

	}

	g_last_head=list_maps;
	return list_maps;
}

/**
 * pmparser_next
 * @description move between areas
 */
procmaps_struct* pmparser_next(){
	if(g_last_head==NULL) return NULL;
	if(g_current==NULL)
		g_current=g_last_head;
	else
		g_current=g_current->next;

	return g_current;
}

/**
 * pmparser_free
 * @description should be called at the end to free the resources
 * @param maps_list the head of the list to be freed
 */
void pmparser_free(procmaps_struct* maps_list){
	if(maps_list==NULL) return ;
	procmaps_struct* act=maps_list;
	procmaps_struct* nxt=act->next;
	while(act!=NULL){
		free(act);
		act=nxt;
		if(nxt!=NULL)
			nxt=nxt->next;
	}
}
