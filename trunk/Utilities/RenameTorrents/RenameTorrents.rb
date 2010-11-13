#!/opt/local/bin/ruby

require 'find'
require 'fileutils'


$invalid_characters = "[]{}()=+-',"


def has_invalid_chacters(path)
    if path.index(' ') != nil
        return true
    end

    $invalid_characters.each_byte do |c|
        if path.index(c.chr) != nil
            return true
        end
    end
    return false
end


def renamed_file(path)
    new_path = path
    $invalid_characters.each_byte do |c|
        regex = "\\#{c.chr}"
        new_path = new_path.gsub(/#{regex}/, "").gsub(/ /, "_")
    end
    return new_path
end


def find_invalid_path(dir)
    Find.find(dir) do |path|
        if FileTest.directory?(path)
            if File.basename(path)[0] == ?. and File.basename(path) != '.'
                Find.prune
            elsif has_invalid_chacters(path)
                return path
            else
                next
            end
        elsif has_invalid_chacters(path)
            return path
        end
    end
    return nil
end

def rename_files(dir)
    path = find_invalid_path(dir)
    while path != nil do
        FileUtils.mv(path, renamed_file(path))
        path = find_invalid_path(dir)
    end
end

rename_files(ARGV[0])


