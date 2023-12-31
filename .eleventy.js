module.exports = function(eleventyConfig) {
	eleventyConfig.addPassthroughCopy("eleventy-src/style.css");
	eleventyConfig.addPassthroughCopy("eleventy-src/script.js");
	eleventyConfig.addPassthroughCopy({
		"previews/geometry-demo.png": "og-image.png"
	});
  // Set custom directories for input, output, includes, and data
  return {
		passthroughFileCopy: true,
    dir: {
      input: "eleventy-src",
      includes: "_includes",
      data: "_data",
      output: "docs"
    }
  };
};