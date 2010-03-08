automaticSync = false;
tocSupported = false;

/**
 * Initialize TOC by showing current document.
 */
function initToc() {
	var id = String(location);
	if (id.indexOf("#") != -1) {
		id = id.substring(id.indexOf("#") + 1);
		var li = document.getElementById(id);
		if (li != null) {
			addClass(li, "on");
		}
	}
}