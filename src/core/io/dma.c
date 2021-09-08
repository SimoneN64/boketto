#include <dma.h>
#include <log.h>
#include <string.h>
#include <mem.h>

void init_dma(dmac_t* dmac) {
  dmac->channels[0].unit_count_mask = 0x3FFF;
  dmac->channels[1].unit_count_mask = 0x3FFF;
  dmac->channels[2].unit_count_mask = 0x3FFF;
  dmac->channels[3].unit_count_mask = 0xFFFF;
}

void set_dma_channel_control(mem_t* mem, u8 i, u16 val) {
  dma_channel_t* channel = &mem->dmac.channels[i];
  dmacnt_h_t prev = channel->control;
  channel->control.raw = val;

  if (!channel->control.enable) {
    channel->start_timing = NONE;
    return;
  }

  if (channel->control.start_timing == 3) {
    switch (i) {
      case 0: logfatal("Invalid special timing for DMA0"); break;
      case 3: channel->start_timing = VIDEO_CAPTURE; break;
      default: channel->start_timing = SOUND; break;
    }
  } else {
    channel->start_timing = channel->control.start_timing;
  }
 
  const u8 unit_size_shift = 1 + channel->control.type;
  
  switch (channel->control.src_ctrl) {
    case 0: channel->sad_increment = 1 << unit_size_shift; break; // Increment
    case 1: channel->sad_increment = -1 << unit_size_shift; break; // Decrement
    case 2: channel->sad_increment = 0; break; // Fixed
    case 3: logfatal("Invalid DMA src increment mode 3"); // Invalid
  }
  
  switch (channel->control.dest_ctrl) {
    case 0: case 3: channel->dad_increment = 1 << unit_size_shift; break; // Increment (mode 3 = reload on repeat)
    case 1: channel->dad_increment = -1 << unit_size_shift; break; // Decrement
    case 2: channel->dad_increment = 0; break; // Fixed
  }
  
  if (channel->start_timing == IMMEDIATE) {
    channel->remaining_units = channel->unit_count & channel->unit_count_mask;
    channel->current_sad = channel->sad;
    channel->current_dad = channel->dad;
    if (channel->control.type) {
      do {
        write_32(mem, 0xDEADBEEF, channel->current_dad, read_32(mem, 0xCAFEBABE, channel->current_sad));
        logdebug("[INFO][DMA] Source address: %08X Destination address: %08X Remaining units: %08X\n", channel->current_sad, channel->current_dad, channel->remaining_units);
        channel->current_sad += channel->sad_increment;
        channel->current_dad += channel->dad_increment;
        channel->remaining_units = (channel->remaining_units - 1) & channel->unit_count_mask;
      } while (channel->remaining_units);
    } else {
      do {
        write_16(mem, 0xDEADCAFE, channel->current_dad, read_16(mem, 0xCAFEBEEF, channel->current_sad));
        logdebug("[INFO][DMA] Source address: %08X Destination address: %08X Remaining units: %08X\n", channel->current_sad, channel->current_dad, channel->remaining_units);
        channel->current_sad += channel->sad_increment;
        channel->current_dad += channel->dad_increment;
        channel->remaining_units = (channel->remaining_units - 1) & channel->unit_count_mask;
      } while (channel->remaining_units);
    }
  }
}

u8 read8_io_dma(mem_t* mem, u32 addr) {
  switch(addr & 0xff) {
    case 0xBA: return mem->dmac.channels[0].control.raw & 0xff;
    case 0xBB: return mem->dmac.channels[0].control.raw >> 8;
    case 0xC6: return mem->dmac.channels[1].control.raw & 0xff;
    case 0xC7: return mem->dmac.channels[1].control.raw >> 8;
    case 0xD2: return mem->dmac.channels[2].control.raw & 0xff;
    case 0xD3: return mem->dmac.channels[2].control.raw >> 8;
    case 0xDE: return mem->dmac.channels[3].control.raw & 0xff;
    case 0xDF: return mem->dmac.channels[3].control.raw >> 8;
  }
}

u16 read16_io_dma(mem_t* mem, u32 addr) {
  switch(addr & 0xff) {
    case 0xBA: return mem->dmac.channels[0].control.raw;
    case 0xC6: return mem->dmac.channels[1].control.raw;
    case 0xD2: return mem->dmac.channels[2].control.raw;
    case 0xDE: return mem->dmac.channels[3].control.raw;
  }
}

u32 read32_io_dma(mem_t* mem, u32 addr) {
  switch(addr & 0xff) {
    case 0xB8: return mem->dmac.channels[0].control.raw << 16;
    case 0xC4: return mem->dmac.channels[1].control.raw << 16;
    case 0xD0: return mem->dmac.channels[2].control.raw << 16;
    case 0xDC: return mem->dmac.channels[3].control.raw << 16;
  }
}

void write8_io_dma(mem_t* mem, u32 addr, u8 val) {
  switch(addr & 0xff) {
    case 0xBA: set_dma_channel_control(mem, 0, (mem->dmac.channels[0].control.raw & 0xff00) | val); break;
    case 0xBB: set_dma_channel_control(mem, 0, (mem->dmac.channels[0].control.raw & 0x00ff) | (val << 8)); break;
    case 0xC6: set_dma_channel_control(mem, 1, (mem->dmac.channels[1].control.raw & 0xff00) | val); break;
    case 0xC7: set_dma_channel_control(mem, 1, (mem->dmac.channels[1].control.raw & 0x00ff) | (val << 8)); break;
    case 0xD2: set_dma_channel_control(mem, 2, (mem->dmac.channels[2].control.raw & 0xff00) | val); break;
    case 0xD3: set_dma_channel_control(mem, 2, (mem->dmac.channels[2].control.raw & 0x00ff) | (val << 8)); break;
    case 0xDE: set_dma_channel_control(mem, 3, (mem->dmac.channels[3].control.raw & 0xff00) | val); break;
    case 0xDF: set_dma_channel_control(mem, 3, (mem->dmac.channels[3].control.raw & 0x00ff) | (val << 8)); break;
  }
}

void write16_io_dma(mem_t* mem, u32 addr, u16 val) {
  switch(addr & 0xff) {
    case 0xB8: mem->dmac.channels[0].unit_count = val; break;
    case 0xBA: set_dma_channel_control(mem, 0, val); break;
    case 0xC4: mem->dmac.channels[1].unit_count = val; break;
    case 0xC6: set_dma_channel_control(mem, 1, val); break;
    case 0xD0: mem->dmac.channels[2].unit_count = val; break;
    case 0xD2: set_dma_channel_control(mem, 2, val); break;
    case 0xDC: mem->dmac.channels[3].unit_count = val; break;
    case 0xDE: set_dma_channel_control(mem, 3, val); break;
  }
}

void write32_io_dma(mem_t* mem, u32 addr, u32 val) {
  switch(addr & 0xff) {
    case 0xB0: mem->dmac.channels[0].sad = val; break;
    case 0xB4: mem->dmac.channels[0].dad = val; break;
    case 0xB8:
      mem->dmac.channels[0].unit_count = val & 0xffff;
      set_dma_channel_control(mem, 0, val >> 16);
      break;
    case 0xBC: mem->dmac.channels[1].sad = val; break;
    case 0xC0: mem->dmac.channels[1].dad = val; break;
    case 0xC4:
      mem->dmac.channels[1].unit_count = val & 0xffff;
      set_dma_channel_control(mem, 1, val >> 16);
      break;
    case 0xC8: mem->dmac.channels[2].sad = val; break;
    case 0xCC: mem->dmac.channels[2].dad = val; break;
    case 0xD0:
      mem->dmac.channels[2].unit_count = val & 0xffff;
      set_dma_channel_control(mem, 2, val >> 16);
      break;
    case 0xD4: mem->dmac.channels[3].sad = val; break;
    case 0xD8: mem->dmac.channels[3].dad = val; break;
    case 0xDC:
      mem->dmac.channels[3].unit_count = val & 0xffff;
      set_dma_channel_control(mem, 3, val >> 16);
      break;
  }
}
