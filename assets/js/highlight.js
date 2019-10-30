$("table").addClass("table");

// Three or more tildes are used to fence off code blocks
// kramdown adds the "highlight" class to the pre elements used 
// to display code blocks. For example, this markdown
//
// ~~~~~~
// test
// ~~~~~~
//
// would generate this html (splitting across lines and indenting added):
//
// <div class="highlighter-rouge">
//   <div class="highlight">
//     <pre class="highlight">
//       <code>
//         test
//       </code>
//     </pre>
//   </div>
// </div>
//

// The polcodesnippet and polsnippet filters convert "<" and ">" into HTML
// entities &lt; and &gt; and add <span> tags to highlight specified lines.
// The following processes the text as HTML (by assigning it to innerHTML) 
// so that it will not be displayed literally.
var _pre = document.querySelectorAll('pre.highlight');
for (var i = 0, element; element = _pre[i]; i++) {
    var lines = element.textContent.split("\n").slice(0,-1);
    element.innerHTML=lines.join("\n");
}
