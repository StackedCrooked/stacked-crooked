require 'FFMPEG.rb'

# Provides an interface to video conversion utilities
class VideoConverter
  def initialize(video)
    @converter = FFMPEG.new
    @video = File.dirname(__FILE__) + "/../TestVideos/" + video
    @progress = 0
    @duration = 1
    @mutex = Mutex.new
    @cv = ConditionVariable.new
  end

  # Returns the duration in seconds for a given video file.
  def get_duration
    @duration = @converter.get_duration(@video)
    if @duration == 0
      @duration = 1
    end
    @duration
  end

  def get_progress
    result = 0
    @mutex.synchronize do
      @cv.wait(@mutex)
      result = @progress
    end
    result
  end

  # Executes a simple ffmpeg convert command of the form <tt>ffmpeg -i <inputfile> <outputfile></tt>
  # +input_file+:: path to the input video file
  # +output_file+:: path for the output video file
  # +progress_handler+:: +Proc+ object that takes a progress value (in seconds).
  def convert_to(output_file)
    @out_video = File.dirname(__FILE__) + "/" + output_file
    Thread.new do
      @converter.convert(@video, @out_video, Proc.new do |progress|
        @mutex.synchronize do
          @progress = progress
          @cv.signal
        end
      end)
    end
  end
end

