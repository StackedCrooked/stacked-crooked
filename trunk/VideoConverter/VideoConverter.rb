require 'FFMPEG.rb'

# VideoConverter enables converting of videos. It is based on FFMPEG.
class VideoConverter

  # VideoConverter constructor.
  # +video+:: identifier for the to be converted video file
  def initialize(video)
    @converter = FFMPEG.new
    @video = File.dirname(__FILE__) + "/../TestVideos/" + video
    @progress = 0
    @mutex = Mutex.new
    @cv = ConditionVariable.new
  end

  # Returns duration of the video file in seconds.
  def get_duration
    if not @duration
      @duration = @converter.get_duration(@video)
    end
    @duration
  end

  # Returns the progress in seconds of the conversion process.
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

  # Converts the video. The target codec is deduced from the +ouput_file+'s file extension.
  # +output_file+:: name of the output video file
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

