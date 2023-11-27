function updateEmbeddedWebsite() {
    // Get the iframe element
    var iframe = document.getElementById("weather_embedded");

    // Reload the iframe's content
    iframe.contentWindow.location.reload(true);
}

// Update the embedded website every 5 minutes (300,000 milliseconds)
setInterval(updateEmbeddedWebsite, 300000);
