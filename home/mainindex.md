---
layout: home
title: Index
---
[//]: # Iterate through index terms stored in alphabetical order in the indexterms array
[//]: # and obtain information for creating the link from indexhash
{% for k in page.indexterms                                                                          %}
    {% assign v = page.indexhash[k]                                                                  %}
    {% assign s = ""                                                                                 %}
    {% for x in v                                                                                    %}
    {%   if forloop.first == false                                                                   %}
    {%     assign s = s | append: ", "                                                               %}
    {%   endif %}
    {%   capture atag %}<a href="{{x['url']}}">{{x['section']}}.{{x['subsection']}}</a>{% endcapture %}
    {%   assign s = s | append: atag                                                                 %}
    {% endfor                                                                                        %}
    {% capture entry                                                                                 %}
        <tr class="index-entry">
        <td>{{ k }}</td>
        <td>{{ s }}</td>
        </tr>
    {% endcapture                                                                                    %}
    {% assign entries = entries | append: entry                                                      %}
{% endfor                                                                                            %}

<div>
<table class="table table-striped" style="width:100%">
<tr class="header">
<td width="20%"><b>Term</b></td>
<td width="75%"><b>Sections</b></td>
</tr>
{{ entries }}
</table>
</div>

<!--     {%   assign s = s | append: x['section'] | append: "." | append: x['subsection'] %} -->
