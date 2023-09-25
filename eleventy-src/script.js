let $containerWrapper;
let $nav;
let $iframe;
let $loader;
let $mobileInfoBtn;
let $listItems;

let mobileInfoOpen = true;

addEventListener("DOMContentLoaded", init);

function init() {
	$containerWrapper = document.getElementById("container-wrapper");
	$nav = document.getElementById("nav");
	$iframe = document.getElementById("iframe");
	$loader = document.getElementById("lds-dual-ring");
	$mobileInfoBtn = document.getElementById("mobile-info");
	$listItems = document.getElementsByClassName("demo-link");

	// $iframe.setAttribute(`width`, `${innerWidth}px`);
	// $iframe.setAttribute(`height`, `${innerHeight}px`);

	window.addEventListener("message", onIframeMessage);
	$iframe.addEventListener("load", onIframeLoad);
	$nav.addEventListener("click", onNavClick);
	$mobileInfoBtn.addEventListener("click", onMobileInfoClick);
}

function onIframeMessage(e) {
	console.log(e.data)
	if (e.data === 'DEMO_INIT') {
		$loader.classList.remove("visible");
	}
}

function onIframeLoad() {
	// ...
}

function onMobileInfoClick() {
	$containerWrapper.classList.toggle("visible");
	mobileInfoOpen = !mobileInfoOpen;
	$mobileInfoBtn.textContent = `${mobileInfoOpen ? `Close` : `Open`} info`;
}

function onNavClick(e) {
	if (e.target.nodeName === "LI") {
		if (e.target.classList.contains("active")) {
			return;
		}
		const demoName = e.target.getAttribute("data-demo-name");
		$iframe.setAttribute("src", `/examples/${demoName}`);
		$loader.classList.add("visible");
		for (let i = 0; i < $listItems.length; i++) {
			$listItems[i].classList.remove("active")
		}
		e.target.classList.add("active");
	}
}