
#include <fat.h>



fat_t get_fat_type(void *bootsector)
{
  bpb *base_bpb = bootsector;
  ebpb32 *ext_bpb32 = bootsector+sizeof(bpb);
  size_t fat_size, total_sectors, root_dir_sectors, data_sectors, num_of_clusters;
  
  if(base_bpb->fat_size != 0)
    fat_size = base_bpb->fat_size;
  else
    fat_size = ext_bpb32->fat_size;
  
  if(base_bpb->num_of_sectors != 0)
    total_sectors = base_bpb->num_of_sectors;
  else
    total_sectors =ext_bpb32->num_of_sectors32;
  
  root_dir_sectors = ((base_bpb->max_root_dir_entries * 32) + (base_bpb->sector_size - 1)) / base_bpb->sector_size;
  data_sectors = total_sectors -(base_bpb->reserved_sectors + (base_bpb->num_of_fats * fat_size) + root_dir_sectors);
  num_of_clusters = data_sectors / base_bpb->sectors_per_cluster;
  
  if(num_of_clusters < 4085)
  {
    return fat12;
  }
  else if(num_of_clusters < 65525)
  {
    return fat16;
  }
    return fat32;
}
