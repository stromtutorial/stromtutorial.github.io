module CPPTutorial
  module Filters
  
    # match_page filter
    #
    # Used by page_ref tag to locate a markdown file named filename
    #
    def match_page(filename)
      filename_regex_pattern = "\/" + filename + "\.md"

      site = @context.registers[:site]
      matches = site.pages.find_all {|page| page.relative_path.match(Regexp.new(filename_regex_pattern)) }

      #puts matches.size.to_i
      
      if matches.size == 1
        return matches[0]
      else 
        raise ArgumentError, "Multiple pages match '#{filename}'"
      end
    end

    # polcodesnippet filter
    #
    # Example: {{ "steps/step-03/src/tree.hpp" | polcodesnippet:"start-endclassdeclaration,end","oneline,rangestart-rangeend"}}
    #   File comes before the pipe in the tag: specify full path back to root of project
    # 
    #   Filter argument linestoshow specifies lines to include in the code snippet
    #   Filter argument linestoemphasize specifies lines to make bold and blue
    # 
    #   In linestoshow, words embedded in 3-slash comments in source files signify range boundaries: e.g. 
    #     ///start
    #     ///endclassdeclaration
    #     ///end
    #
    #   If linestoshow is empty, then all lines will be shown
    #
    #   In linestoemphasize, the 3-slash comments in source files must start with !
    #     ///!oneline, 
    #     ///!rangestart
    #     ///!rangeend
    #
    #   The additional ! makes it possible to specify the same word for both linestoshow and linestoemphasize
    #   In this case, be sure to include the ! in linestoshow. For example, the following would both show
    #   and emphasize all lines between (and including) lines labeled ///!a and ///!b in tree.hpp:
    #      {{ "steps/step-03/src/tree.hpp" | polcodesnippet:"!a-!b","a-b"}}
    #
    #   In non-source-code files (e.g. strom.conf), 3-hash comments are used: e.g.
    #     ###start
    #     ###end
    #     ###!emph
    #
    def polcodesnippet(file,linestoshow,linestoemphasize)
        site = @context.registers[:site]

        # determine ranges to show
        showstart = []
        showend = []
        showsingle = []
        for s in linestoshow.split(',')
            r = s.split('-')
            if r.length == 1
                showsingle.push(r.first)
            elsif r.length == 2
                showstart.push(r.first)
                showend.push(r.last)
            else
                raise ArgumentError, "An entry in lines to show invalid in 'snippet' filter"
            end
        end

        # determine ranges to emphasize
        emphasizestart = []
        emphasizeend = []
        emphasizesingle = []
        for s in linestoemphasize.split(',')
            r = s.split('-')
            if r.length == 1
                emphasizesingle.push(r.first)
            elsif r.length == 2
                emphasizestart.push(r.first)
                emphasizeend.push(r.last)
            else
                raise ArgumentError, "An entry in lines to show invalid in 'snippet' filter"
            end
        end

        content = File.read(file)
        
        # main loop building up ret string line by line
        returned_content = ""
        showing = false
        emphasizing = false
        for line in content.each_line do
            # Change < and > to corresponding html entities &lt; and &gt;
            line = line.gsub(/\</,'&lt;').gsub(/\>/,'&gt;')

            tag = line.match('(///|###)(\S+)') {|m| m[2]}
            emph = tag && tag[0] == '!' ? tag[1..-1] : nil
    
            if linestoshow.length == 0 || (showstart.include? tag) || (showstart.include? emph)
                showing  = true
            end
    
            if emphasizestart.include? emph
                emphasizing = true
            end
    
            if showing || (showsingle.include? tag) || (showsingle.include? emph)
                line = line.gsub(/\/\/\/\S+/, '')
                line = line.gsub(/###\S+/, '')
                if emphasizing || (emphasizesingle.include? emph)
                    leadingspace = line[/\s*/]
                    returned_content += leadingspace + "<span style=\"color:#0000ff\"><strong>" + line.strip + "</strong></span>\n"
                else
                    returned_content += line
                end
            end

            if showend.include? tag
                showing  = false
            end
    
            if emphasizeend.include? emph
                emphasizing = false
            end
        end

        returned_content
    end

    # polsnippet filter
    #
    # Similar to polcodesnippet filter except line numbers are used to specify lines
    # and ranges of lines rather than key words.
    #
    # Example: {{ "steps/step-03/src/tree.hpp" | polsnippet:"1-173","30,51-52,84-172"}}
    #
    #   File comes before the pipe in the tag: specify full path back to root of project
    # 
    #   Filter argument linestoshow specifies lines to include in the code snippet
    #   Filter argument linestoemphasize specifies lines to make bold and blue
    #
    def polsnippet(file,linestoshow,linestoemphasize)
        site = @context.registers[:site]

        content = File.read(file)

        # determine ranges to show
        showing = []
        for s in linestoshow.split(',')
            r = s.split('-')
            if r.length == 1
                showing.push(r.first.to_i)
            elsif r.length == 2
                from = r.first.to_i
                to = r.last.to_i
                if from > to
                    raise ArgumentError, "Lines to show from > to in 'snippet' filter"
                end
                if to > content.lines.count
                    raise ArgumentError, "Lines to show out of range in 'snippet' filter"
                end
                for ss in from..to
                    showing.push(ss)
                end
            else
                raise ArgumentError, "An entry in lines to show invalid in 'snippet' filter"
            end
        end

        # determine ranges to emphasize
        emphasize = []
        for s in linestoemphasize.split(',')
            r = s.split('-')
            if r.length == 1
                emphasize.push(r.first.to_i)
            elsif r.length == 2
                from = r.first.to_i
                to = r.last.to_i
                if from > to
                    raise ArgumentError, "Lines to emphasize from > to in 'snippet' filter"
                end
                if to > content.lines.count
                    raise ArgumentError, "Lines to emphasize out of range in 'snippet' filter"
                end
                for ss in from..to
                    emphasize.push(ss)
                end
            else
                raise ArgumentError, "An entry in lines to emphasize invalid in 'snippet' filter"
            end
        end

        # main loop building up ret string line by line
        returned_content = ""
        i = 1
        for line in content.each_line do
            # Change < and > to corresponding html entities &lt; and &gt;
            line = line.gsub(/\</,'&lt;').gsub(/\>/,'&gt;')
            if showing.include? i
                if emphasize.include? i
                    leadingspace = line[/\s*/]
                    returned_content += leadingspace + "<span style=\"color:#0000ff\"><strong>" + line.strip + "</strong></span>\n"
                else
                    returned_content += line
                end
            end
            i += 1
        end

        returned_content
    end

  end
end

Liquid::Template.register_filter(CPPTutorial::Filters)
