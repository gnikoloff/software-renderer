let $nav
let $iframe

addEventListener("DOMContentLoaded", init);

function init() {
	$nav = document.getElementById("nav");
	$iframe = document.getElementById("iframe");

	$nav.addEventListener("click", onNavClick);
}

function onNavClick(e) {
	if (e.target.nodeName === "LI") {
		const demoName = e.target.getAttribute("data-demo-name");
		$iframe.setAttribute("src", `/examples/${demoName}`);
	}
}