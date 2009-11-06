require 'FFMPEG.rb'

# Provides an interface to video conversion utilities
class VideoConverter
  def initialize
    @converter = FFMPEG.new
    @progress = 0
    @duration = 1
  end

  def get_progress
    return @progress/@duration
  end

  def get_file(vid)
     return File.dirname(__FILE__) + "/../TestVideos/" + vid
  end

  def get_out_file(id)
    return File.dirname(__FILE__) + "/" + id
  end
  
  

  # Returns the duration in seconds for a given video file.
  def get_duration(vid)
    return @converter.get_duration(get_file(vid))
  end

  # Executes a simple ffmpeg convert command of the form <tt>ffmpeg -i <inputfile> <outputfile></tt>
  # +input_file+:: path to the input video file
  # +output_file+:: path for the output video file
  # +progress_handler+:: +Proc+ object that takes a progress value (in seconds).
  def convert(vid, output_file)
    @duration = get_duration(vid)
    if @duration == 0
      @duration = 1
    end
    @converter.convert(get_file(vid),
                       get_out_file(output_file),
                       Proc.new { |progress| @progress = progress })
  end
end

