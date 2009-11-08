require 'FFMPEG.rb'
require 'thread'

# VideoConverter enables converting of videos. It is based on FFMPEG.
class VideoConverter

  # VideoConverter constructor.
  # +video+:: identifier for the to be converted video file
  def initialize(video)
    if not File.exist?(video)
      raise "File #{video} does not exist."
    end
    @video = video
    @converter = FFMPEG.new
    @progress = 0
    @mutex = Mutex.new
    @done = false
  end

  def done
    res = false
    @mutex.synchronize do
      res = @done
    end
    res
  end

  # Returns duration of the video file in seconds.
  def get_duration
    res = 0
    @mutex.synchronize do
      @duration = @converter.get_duration(@video)
      res = @duration
    end
    res
  end

  # Returns the progress in seconds of the conversion process.
  def get_progress
    res = 0
    if not done()
      @mutex.synchronize do
        res = @progress
      end
    else
      res = 100
    end
    res
  end

  # Converts the video. The target codec is deduced from the +ouput_file+'s file extension.
  # This method is non-blocking, you can check if the job is done with the +done()+ method.
  # +output_file+:: name of the output video file
  def convert_to(output_file)
    if File.exist?(output_file)
      raise "Output file already exists: #{output_file}"
    end
    @out_video = output_file
    Thread.new do
      @converter.convert(@video, @out_video, Proc.new do |progress|
        @mutex.synchronize do
          if @progress < @duration
            @progress = (0.5 + ((100 * progress).to_f / get_duration().to_f)).to_i
          else
            @progress = 100
          end
        end
      end)

      @mutex.synchronize do
        @done = true
      end
    end
  end
end

