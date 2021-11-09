![ofxAdvatekAssistor](ofxaddons_thumbnail.png)
==================

OpenFrameworks addon to get data from and to [Advatek Pixel Controllers].

It has no UI, see [Advatek-Assistant] for a UI build with [ofxImGUI]


How to Use
==========

    1. Advatek::Poll();               // Populate a list of `sAdvatekDevice`
    2. Advatek::getDevices();         // Get the current connected devices (No polling)
    
    3. // Update the configuration of any `sAdvatekDevice` in the list
    
    4. Advatek::updateDevice(int d);  // Sends the updated config to the device based on list index


Dependency
==========

  - [ofxNetwork]

[Advatek Pixel Controllers]: https://www.advateklights.com/products/pixel-control
[ofxImGUI]: https://github.com/jvcleave/ofxImGui
[Advatek-Assistant]: https://github.com/studioENESS/Advatek-Assistant
[ofxNetwork]: https://github.com/openframeworks/openFrameworks/tree/master/addons/ofxNetwork
