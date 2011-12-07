require 'RMagick'
include Magick

img = ImageList.new("luffy-128x128.png");
(0..127).each do |x|
    (0..127).each do |y|
        if x < 16 || y < 16 || x >= 112 || y >= 112
            color = img.pixel_color(x, y)
            color.opacity = 65535
            img.pixel_color(x, y, color)
        end
    end
end

new_file = "luffy-128x128-updated.png"
img.write(new_file)
