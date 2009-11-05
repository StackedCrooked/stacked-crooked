require 'open3'

module ProcessUtils

  # Runs a subprocess and applies handlers for stdout and stderr
  # Params:
  # - command: command line string to be executed by the system
  # - outhandler: proc object that takes a pipe object as first and only param
  # - errhandler: proc object that takes a pipe object as first and only param
  def execute_and_handle(command, outhandler, errhandler)
    Open3.popen3(command) do |_, stdout, stderr|
      if (outhandler)
        outhandler.call(stdout)
      end
      if (errhandler)
        errhandler.call(stderr)
      end
    end
  end
end

