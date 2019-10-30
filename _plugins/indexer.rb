module Jekyll
    class Indexer < Jekyll::Generator
        def generate(site)
            # Create indexhash, a dictionary in which each key is an index term identified in
            # markdown using the indexshow, indexhide, or indexbold liquid tags and the value
            # is an array of hashes each of which contains the step page's url, its section,
            # and its subsection
            doindex = site.config["doindex"]
            if doindex == true
                puts "~~> In indexer plugin: doindex is true"
                os = site.config["operatingsystem"]
                puts "~~> In indexer plugin: os is " + os
                indexhash = {}
                for p in site.pages
                    #puts "~~> In indexer plugin: processing " + p.url
                    page_section = p.data['partnum']
                    page_subsection = p.data['subpart']

                    # Must strip out parts of p.content that are specific to other operating systems
                    # before scanning for index entries, otherwise (for example) "meson" will show up
                    # in the index for the mac version
                    if os == "linux"
                        # note: assuming linux comes first in each conditional, followed by mac and then win
                        # strip out mac- and windows-specific code before scanning for index entries
                        content = p.content.gsub(/\{% elsif OS == "mac" %\}(.+?)(\{% endif %\})/m, "\\2")
                    elsif os == "mac"
                        # note: assuming linux comes first in each conditional, followed by mac and then win
                        # strip out linux-specific code before scanning for index entries
                        content = p.content.gsub(/\{% if OS == "linux" %\}(.+?)(\{% elsif OS == "mac" %\})/m, "{% if OS == \"mac\" %}")
                        # strip out windows-specific code before scanning for index entries
                        content = content.gsub(/\{% elsif OS == "win" %\}(.+?)(\{% endif %\})/m, "\\2")
                    elsif os == "win"
                        # note: assuming linux comes first in each conditional, followed by mac and then win
                        # strip out linux- and mac-specific code before scanning for index entries
                        content = p.content.gsub(/\{% if OS == "linux" %\}(.+?)(\{% elsif OS == "windows" %\})/m, "\\2")
                    end

                    # content variable now contains content specific to the platform specified
                    allmatches = content.scan /\{%\s+(?:indexshow|indexhide|indexbold|indexfile)\s+(.+?)\s+%\}/
                    allmatches.each do |m|
                        index_term = m[0]
                        url = "/" + os + p.url
                        if indexhash[index_term]
                            indexhash[index_term] << {'url' => url, 'section' => page_section, 'subsection' => page_subsection, 'term' => index_term}
                        else
                            indexhash[index_term] = [{'url' => url, 'section' => page_section, 'subsection' => page_subsection, 'term' => index_term}]
                        end
                    end
                end
            
                # Find the index page (the page that will show the list of index terms and
                # links to where they are in the tutorial). The markdown for the index page is
                # here: /home/mainindex.md
                indexpage = site.pages.detect {|p| p.data["title"] == "Index"}

                # Create an alphabetical list of index terms
                indexterms = indexhash.keys
                indexterms.sort! { |a,b| a.downcase <=> b.downcase }
                indexterms.each do |t|
                    indexhash[t].sort_by! { |a| [a["section"],a["subsection"]] }
                    indexhash[t] = indexhash[t].uniq
                end

                # Store indexterms and indexhash in the data for the index page
                indexpage.data["indexterms"] = indexterms
                indexpage.data["indexhash"] = indexhash
            else
                puts "~~> In indexer plugin: doindex is false"
            end
        end
    end

    Jekyll::Hooks.register :site, :pre_render do |content,payload|
            # Create a "page map" hash in which a key is the page's partnum.subpart
            # (e.g. 2.3) and the corresponding value is the page's URL. This will allow
            # us to create links for every page pointing to the previous and next step.
            os = payload.site["operatingsystem"]
            if os == 'none'
                os = ''
            else
                os += '/'
            end
            
            puts '==> os = ' + os
            
            if !payload.site.key? 'pagemap'
                #puts 'Creating payload.site["pagemap"]...'
                payload.site['pagemap'] = {}
                payload.site.pages.each do |p|
                    x = p.data['partnum']
                    y = p.data['subpart']
                    if x and y
                        k = "#{x}.#{y}"
                        url = "/" + os + p.url
                        #url = "/" + p.url
                        payload.site['pagemap'].store(k, url)
                    end
                end
            end
        end

    Jekyll::Hooks.register :pages, :pre_render do |content, payload|
            #puts "~~> pre_render = #{payload.site['baseurl']}"

            # Store the step part and subpart (e.g. 2.3) as integers
            # so that we can do addition and subtraction.
            part = payload.page['partnum'].to_i
            subpart = payload.page['subpart'].to_i
            
            os = payload.site['operatingsystem']
            if os == 'none'
                os = ''
            else
                os = '/' + os
            end

            # Create link to previous step or substep
            # Decrement subpart to see if there is previous subpart to the current step
            if subpart > 0
                prevpart = part
                prevsubpart = subpart - 1
                key = "#{prevpart}.#{prevsubpart}"
                payload.page['prevlink'] = "<a href=\"#{payload.site['baseurl']}/#{payload.site['pagemap'][key]}\">&lt;&nbsp;#{key}</a>"
            else
                if part > 1
                    prevpart = part - 1
                    prevsubpart = 0
                    key = "#{prevpart}.#{prevsubpart}"
                    payload.page['prevlink'] = "<a href=\"#{payload.site['baseurl']}/#{payload.site['pagemap'][key]}\">&lt;&nbsp;#{key}</a>"
                else
                    payload.page['prevlink'] = "<a href=\"#{payload.site['baseurl']+os}/steps/\">Table of Contents</a>"
                end
            end

            # Create link to next step or substep
            # Increment subpart to see if there is another subpart to the current step
            nextpart = part
            nextsubpart = subpart + 1
            if payload.site.pages.detect {|p| p.data["partnum"] == nextpart and p.data["subpart"] == nextsubpart}
                # A page having step number part.(subpart + 1) was found
                key = "#{nextpart}.#{nextsubpart}"
                payload.page['nextlink'] = "<a href=\"#{payload.site['baseurl']}/#{payload.site['pagemap'][key]}\">#{key}&nbsp;&gt;</a>"
            else
                # No page having step number part.(subpart + 1) was found, so try incrementing part instead
                nextpart = part + 1
                nextsubpart = 0
                if payload.site.pages.detect {|p| p.data["partnum"] == nextpart and p.data["subpart"] == nextsubpart}
                    # A page having step number (part + 1).0 was found
                    key = "#{nextpart}.#{nextsubpart}"
                    payload.page['nextlink'] = "<a href=\"#{payload.site['baseurl']}/#{payload.site['pagemap'][key]}\">#{key}&nbsp;&gt;</a>"
                else
                    # No page having step number (part + 1).0 was found, so we must be at the end
                    payload.page['nextlink'] = "<a href=\"#{payload.site['baseurl']+os}/steps/\">Table of Contents</a>"
                end
            end

            #puts "~~> pre_render = #{payload.page['partnum']}.#{payload.page['subpart']} #{payload.page['title']} (#{payload.page['nextlink']})"
        end

end
