require 'FFMPEG.rb'

#
# Configuration
#
$input_file = "../TestVideos/#{ARGV[0]}"
$output_format = "flv"
$output_file = "output.#{$output_format}"

#
# Script
#
converter = FFMPEG.new


# Remove old output file
converter.execute_and_handle("rm #{$output_file}", nil, nil)

# Get duration
duration = converter.get_duration($input_file)

# Convert and report progress
converter.convert($input_file,
                  $output_file,
                  Proc.new { |progress| puts((progress/duration).to_s + "%") } )

# Done
puts "Done!"

