# harfbuzz_textdrawing_sample
A sample project that uses Harfbuzz library to draw text on stdout.

## Issue glyphs drawn overlapped on latest version
when dig deep we found that with this code issue occure 

```
hb_font_t *font = hb_ft_font_create(face, NULL);
hb_font_t *hb_font = hb_font_create_sub_font(font);
hb_font_destroy(font);
```   
but with (no issue)
```
hb_font_t *hb_font = hb_ft_font_create(face, NULL);
```


**Example(T+e):**

```
#########
#########
#########
   +#
   +#
   +#
   +# ###
   +######
   +######
   +##+ +#+
   +##   ##
   +#+   +#
   +#    +#
   +#######
   +#######
   +#######
   +#
   +#
   +#+
   +##    +
   +##+  #+
   +######+
   +######
   +# ###
```
