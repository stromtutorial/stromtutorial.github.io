require './_plugins/filters.rb'

module CPPTutorial
    module Tags
        class PageRef < Liquid::Tag
            include CPPTutorial::Filters

            def initialize(tag_name, markup, tokens)
				super
			end

            def render(context)
                @context = context
                site = @context.registers[:site]

                page = match_page(@markup.strip)
                os = @context.registers[:site].config['operatingsystem']
                if os == 'none'
                    "<a href=\"#{site.baseurl+page.url}\">#{page['title']}</a>"
                else
                    "<a href=\"#{site.baseurl}\/#{os+page.url}\">#{page['title']}</a>"
                end

            end
        end

    end
end

Liquid::Template.register_tag('page_ref', CPPTutorial::Tags::PageRef)
