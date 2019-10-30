require "jekyll"
require "gemoji"
require "html/pipeline"

module Jekyll
    Emoji.create("blueproject") do |char|
      char.image_filename = "blueproject.png"
      char.add_tag "xcode"
    end
    Emoji.create("yellowfolder") do |char|
      char.image_filename = "yellowfolder.png"
      char.add_tag "xcode"
    end
end
