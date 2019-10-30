---
layout: home
title: Step-by-step instructions
subpart: 0
permalink: /steps/
---
The steps below are designed to be visited in order.

{% assign steps = site.pages | where:"layout","steps" | where:"subpart",0 | sort: "partnum" %}

{% for step in steps %}
	{% capture part %}
		<tr class="part" id="{{ step.partnum }}">
		<td>Step {{ step.partnum }}</td>
		<td><a href="{{ site.baseurl }}{{ step.url }}">{{ step.title }}</a></td>
		<td>{{ step.description | markdownify }}</td>
		</tr>
	{% endcapture %}

	{% assign parts = parts | append: part %}
{% endfor %}

<div>
<table class="table table-striped" style="width:100%">
<tr class="header">
<td width="15%"><b>Step</b></td>
<td width="25%"><b>Title</b></td>
<td width="50%"><b>Description</b></td>
</tr>
{{ parts }}
</table>
</div>

