"use strict";

module.exports = {
    "buckets": {
        "satellite": {
            "source": "satellite",
            "type": "raster"
        }
    },
    "classes": [
        {
            "layers": {
                "satellite": {
                    "opacity": 1
                }
            },
            "name": "default"
        }
    ],
    "sprite": "/img/maki-sprite",
    "structure": [
        {
            "bucket": "satellite",
            "name": "satellite"
        }
    ]
};
