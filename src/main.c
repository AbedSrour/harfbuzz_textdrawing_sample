/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "hb.h"
#include "hb-ft.h"

#define WIDTH 150
#define HEIGHT 64

#define DEBUG_ENABLE

unsigned char print_glyphs = 0;
// #define PRINT_GLYPHS

#ifdef DEBUG_ENABLE
#define DEBUG printf
#else
#define DEBUG
#endif

typedef struct
{
   unsigned int codepoint;
   unsigned char *buffer;
   unsigned int width;
   unsigned int height;
   float bearing_x;
   float bearing_y;
} Glyph;

/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

void print_glyph_pos(hb_glyph_position_t *gp) {
#ifdef DEBUG_ENABLE
   printf("Glyph pos:\n var: %d\n x_adv: %d\n y_adv: %d\n x_off: %d\n y_off: %d\n", gp->var.i32, gp->x_advance, gp->y_advance, gp->x_offset, gp->y_offset);
#endif
}

/* Replace this function with something useful. */

void draw_bitmap(Glyph *glyph, float x, float y, float xo, float yo)
{
   FT_Int i, j, p, q;
   FT_Int x_max; // = x + bitmap->width;
   FT_Int y_max; // = y + bitmap->rows;

   float x0 = x + xo + glyph->bearing_x;
   float y0 = floor(y + yo + glyph->bearing_y);
   float x1 = x0 + glyph->width;
   float y1 = floor(y0 + glyph->height);
   DEBUG("\nDrawing: glyph# %i\n", glyph->codepoint);
   DEBUG("bearing:x(%.2f)y(%.2f)\n", glyph->bearing_x, glyph->bearing_y);
   DEBUG("offset:x(%.2f)y(%.2f)\n", xo, yo);
   DEBUG("p0(%.1f, %.1f), p1(%.1f, %.1f)\n\n", x0, y0, x1, y1);

   x_max = x0 + glyph->width;
   y_max = y0 + glyph->height;



   if ( print_glyphs ) {
      for (unsigned int iy = 0; iy < glyph->height; ++iy)
      {
         for (unsigned int ix = 0; ix < glyph->width; ++ix)
         {
            int c = (int)glyph->buffer[iy * glyph->width + ix];
            // printf("%c", (c == 255 ? '#' : '`'));
            DEBUG("%c", (c <= 64 ? ' ' : (c >= 150 ) ? '#' : '+'));
         }
         DEBUG("\n");
      }
      DEBUG("\n");
   }

   /* for simplicity, we assume that `bitmap->pixel_mode' */
   /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */

   for (i = x0, p = 0; i < x_max; i++, p++)
   {
      j = 32;
      j -= y0;
      y_max = j + glyph->height;

      for ( q = 0; j < y_max; j++, q++)
      {
         if (i < 0 || j < 0 ||
             i >= WIDTH || j >= HEIGHT)
            continue;

         image[j][i] |= glyph->buffer[q * glyph->width + p];
      }
   }

   DEBUG("\nEnd drawing\n");
}

void show_image(void)
{
   if ( print_glyphs ) {
      int i, j;
      for (i = 0; i < HEIGHT; i++)
      {
         for (j = 0; j < WIDTH; j++) {
            unsigned char c = image[i][j];

            putchar((c <= 64 ? ' ' : (c >= 150 ) ? '#' : '+'));
         }
         putchar('\n');
      }
   }
}

static hb_position_t
_evas_common_font_ot_hb_get_glyph_advance(hb_font_t *font,
      void *font_data, hb_codepoint_t glyph,
      void *user_data)
{
   // /* Use our cache*/
   // RGBA_Font_Int *fi = (RGBA_Font_Int *) font_data;
   // RGBA_Font_Glyph *fg;
   // (void) font;
   // (void) user_data;
   // fg = evas_common_font_int_cache_glyph_get(fi, glyph);
   // if (fg)
   //   {
   //      return fg->glyph->advance.x >> 10;
   //   }
   return 0;
}

static hb_position_t
_evas_common_font_ot_hb_get_kerning(hb_font_t *font, void *font_data,
   hb_codepoint_t first_glyph, hb_codepoint_t second_glyph, void *user_data)
{
   // RGBA_Font_Int *fi = (RGBA_Font_Int *) font_data;
   // int kern;
   // (void) font;
   // (void) user_data;
   // if (evas_common_font_query_kerning(fi, first_glyph, second_glyph, &kern))
   //    return kern;

   return 0;
}

static inline hb_font_funcs_t *
_evas_common_font_ot_font_funcs_get(void)
{
   static hb_font_funcs_t *font_funcs = NULL;
   if (!font_funcs)
     {
        font_funcs = hb_font_funcs_create();
        hb_font_funcs_set_glyph_h_advance_func(font_funcs,
            _evas_common_font_ot_hb_get_glyph_advance, NULL, NULL);
        hb_font_funcs_set_glyph_h_kerning_func(font_funcs,
            _evas_common_font_ot_hb_get_kerning, NULL, NULL);
     }

   return font_funcs;
}


int main(int argc,
         char **argv)
{
   FT_Library library;
   FT_Face face;

   FT_GlyphSlot slot;
   FT_Error error;
   // const char *text = "LVAWeTaY";
   int size = 8;
   const char *text = "Ta";


   char *filename;

   if (argc < 2)
   {
      fprintf(stderr, "usage: %s font-file [text] [size] [-d]\n", argv[0]);
      exit(1);
   }

   if ( argc > 2 ) {
      text = argv[2];
   }

   if ( argc > 3 ) {
      int size_p = atoi(argv[3]);
      if ( size_p ) {
         size = size_p;
      }
   }

   if ( argc > 4 && 0 == strcmp(argv[4], "-d") ) {
      print_glyphs = 1;
   }

   filename = argv[1]; /* first argument     */

   error = FT_Init_FreeType(&library); /* initialize library */
   if (error)
   {
      printf("FT_Init_FreeType: error(%d) %s\n", error, FT_Error_String(error));
      return EXIT_FAILURE;
   }

   error = FT_New_Face(library, filename, 0, &face); /* create face object */
   if (error)
   {
      printf("FT_New_Face: error(%d) %s\n", error, FT_Error_String(error));
      return EXIT_FAILURE;
   }

   error = FT_Set_Char_Size(face, size * 64, 0,
                            HEIGHT, WIDTH); /* set character size */
   if (error)
   {
      printf("FT_Set_Char_Size: error(%d) %s\n", error, FT_Error_String(error));
      return EXIT_FAILURE;
   }

   slot = face->glyph;

   // Harfbuzz configuration
   hb_font_t *font = NULL;
   hb_font_t *hb_ft_font;

   hb_ft_font = hb_ft_font_create(face, NULL);
   font = hb_font_create_sub_font(hb_ft_font);
   hb_font_destroy(hb_ft_font);
   // void *hb_font = hb_font_create_sub_font(font);

   hb_buffer_t *buffer;

   buffer = hb_buffer_create();
   hb_buffer_set_unicode_funcs(buffer, hb_unicode_funcs_get_default());
   hb_buffer_set_language(buffer, hb_language_from_string(NULL, -1));
   hb_buffer_set_script(buffer, HB_SCRIPT_LATIN);
   hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
   size_t length = strlen(text);
   hb_buffer_add_utf8(buffer, text, length, 0, length);

   // hb_buffer_allocation_successful(buffer);

   // hb_buffer_reset(buffer);
   hb_shape(font, buffer, NULL, 0);

   unsigned int glyphCount;
   hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer, NULL);
   hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(buffer, NULL);
   glyphCount = hb_buffer_get_length(buffer);

   float x, y;
   x = 0;
   y = 0;

   for (unsigned int i = 0; i < glyphCount; ++i)
   {
      Glyph glyph;
      error = FT_Load_Glyph(face, glyphInfo->codepoint, FT_LOAD_DEFAULT);
      if (error)
      {
         puts("FT_Load_Glyph error");
         continue;
      }
      slot = face->glyph;
      FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
      FT_Bitmap ftBitmap = slot->bitmap;

      glyph.codepoint = glyphInfo->codepoint;
      glyph.buffer = ftBitmap.buffer;
      glyph.width = ftBitmap.width;
      glyph.height = ftBitmap.rows;
      glyph.bearing_x = slot->bitmap_left;
      glyph.bearing_y = slot->bitmap_top;

# define FONT_ROUND_26_6_TO_INT(x) \
   (((x + 0x20) & -0x40) >> 6)

      printf("\n\nChar @ index %d\n", i);

      print_glyph_pos(glyphPos);

      float xa = (float)FONT_ROUND_26_6_TO_INT(glyphPos->x_advance);
      float ya = (float)FONT_ROUND_26_6_TO_INT(glyphPos->y_advance);
      float xo = (float)FONT_ROUND_26_6_TO_INT(glyphPos->x_offset);
      float yo = (float)FONT_ROUND_26_6_TO_INT(glyphPos->y_offset);

      /* now, draw to our target surface (convert position) */
      draw_bitmap(&glyph, x, y, xo, yo);

      x += xa;
      y += ya;

      glyphPos++;
      glyphInfo++;
   }
   show_image();
   hb_buffer_destroy(buffer);
   hb_font_destroy(font);

   if ((error = FT_Done_Face(face)))
      return EXIT_FAILURE;
   if ((error = FT_Done_FreeType(library)))
      return EXIT_FAILURE;

   return 0;
}

/* EOF */