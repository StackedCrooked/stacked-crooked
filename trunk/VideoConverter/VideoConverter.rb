require 'FFMPEG.rb'

# Provides an interface to video conversion utilities
class VideoConverter
  def initialize(video)
    @converter = FFMPEG.new
    @video = File.dirname(__FILE__) + "/../TestVideos/" + video
    @progress = 0
    @duration = get_duration
    @mutex = Mutex.new
    @cv = ConditionVariable.new
  end

  def get_duration
    if @duration == nil
      @duration = @converter.get_duration(@video)
    end
    @duration
  end

  def get_progress
    result = 0
    if @mutex
      @mutex.synchronize do
        @cv.wait(@mutex)
        result = @progress
      end
    end
    result
  end

  # Executes a simple ffmpeg convert command of the form <tt>ffmpeg -i <inputfile> <outputfile></tt>
  # +input_file+:: path to the input video file
  # +output_file+:: path for the output video file
  # +progress_handler+:: +Proc+ object that takes a progress value (in seconds).
  def convert_to(output_file)
    @out_video = File.dirname(__FILE__) + "/" + output_file
    system("rm #{@out_video}")
    t = Thread.new do
      @converter.convert(@video, @out_video, Proc.new do |progress|
        @mutex.synchronize do
          if @progress < get_duration()
            @progress = (0.5 + ((100 * progress).to_f / get_duration().to_f)).to_i
          elsif
            @progress = 100
          end
          @cv.signal
        end
      end)
      @mutex = nil
    end
  end
end

