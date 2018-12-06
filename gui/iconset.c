

bmp_image_t* iconset_image;

uint8_t
iconset() {
  iconset_image = bmp_image_from_file("ui/ui-icons.bmp");
}
