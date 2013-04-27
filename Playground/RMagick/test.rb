require 'RMagick'
include Magick

img = ImageList.new("luffy-128x128.png");
(0..127).each do |x|
    (0..127).each do |y|
        color = img.pixel_color(x, y)
        print "Original: (#{x},#{y}): #{color.to_s}"
        if x < 16 || y < 16 || x >= 112 || y >= 112
            #color.opacity = 65535
            color.opacity = 65535/2
            print "\tNew (#{x},#{y}): #{color.to_s}"
            img.pixel_color(x, y, color)
        end
        print "\n"
    end
end

new_file = "luffy-128x128-updated.png"
img.write(new_file)
