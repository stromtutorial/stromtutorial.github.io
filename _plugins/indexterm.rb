# ----------------------------------------------------------------------------------------------
# "THE BEER-WARE LICENSE" (Revision 42):
# As long as you retain this notice you can do whatever you want with this stuff.
# If we meet some day, and you think this stuff is worth it, you can buy me a beer in return.
# ----------------------------------------------------------------------------------------------
#
module Jekyll

  class IndexFile < Liquid::Tag
    def initialize(tag_name, text, tokens)
      super
      @termtext = text.strip
    end

    def render(context)
        "*"+@termtext+"*"
    end
  end

  class IndexShow < Liquid::Tag
    def initialize(tag_name, text, tokens)
      super
      @termtext = text.strip
    end

    def render(context)
        @termtext
    end
  end

  class IndexCode < Liquid::Tag
    def initialize(tag_name, text, tokens)
      super
      @termtext = text.strip
    end

    def render(context)
        "`"+@termtext+"`"
    end
  end

  class IndexHidden < Liquid::Tag
    def initialize(tag_name, text, tokens)
      super
    end

    def render(context)
        ""
    end
  end

  class IndexBold < Liquid::Tag
    def initialize(tag_name, text, tokens)
      super
      @termtext = text.strip
    end

    def render(context)
        "<strong>"+@termtext+"</strong>"
    end
  end

end

Liquid::Template.register_tag('indexfile', Jekyll::IndexFile)
Liquid::Template.register_tag('indexcode', Jekyll::IndexCode)
Liquid::Template.register_tag('indexshow', Jekyll::IndexShow)
Liquid::Template.register_tag('indexhide', Jekyll::IndexHidden)
Liquid::Template.register_tag('indexbold', Jekyll::IndexBold)
