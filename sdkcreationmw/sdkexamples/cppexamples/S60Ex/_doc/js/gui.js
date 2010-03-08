/**
 * Check required support for TOC.
 */
function support() {
	if (document.getElementsByTagName && document.styleSheets) {
		tocSupported = true;
	} else {
		//alert("Dynamic TOC not available.");		
	}
}

/** Flag if dynamic TOC is supported */
var tocSupported = false;
support();

/**
 * Hightlight TOC node.
 *
 * @param node TOC node to highlight
 */
function highlightTocItem(node) {
	if (tocSupported) {
		// turn old off
		var lis = node.ownerDocument.getElementsByTagName("li");
		for (var i = 0; i < lis.length; i++) {
			removeClass(lis.item(i), "on3");
		}
		// turn this on
		addClass(node.parentNode, "on3");
	}
}

/**
 * Toggle child node display.
 *
 * @param n Target node of the event.
 */
function toggle(n) {
	// toggle controller
	if (isClass(n, "toc-controller-open")) {
		n.firstChild.data = "+";
		switchClass(n, "toc-controller-closed", "toc-controller-open");
	} else {
		n.firstChild.data = "-";
		switchClass(n, "toc-controller-open", "toc-controller-closed");
	}
	// toggle content	
    i = getNextSiblingByTagName(n.parentNode.firstChild, "ul");
    if (i != null) {
		if (i.style.display == "none") {
			i.style.display = "block";
		} else {
			i.style.display = "none";
		}        
    }
}
