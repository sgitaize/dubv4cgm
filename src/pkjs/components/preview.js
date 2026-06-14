// Static HTML simulation of the watchface for the Clay config UI preview.
// Dimensions are 144x168 (basalt) — intentionally not 1:1 with the target
// emery device (200x228). This is a color-preview mockup, not a device replica.
module.exports = {
  name: 'preview',

  template: '<div style="padding:16px 0;border-bottom:1px solid #666"><div data-preview-root="true" style="position:relative;color:#fff;font-family:DS-Digital,monospace;display:block;width:144px;height:168px;background:#000;border-radius:6px;overflow:hidden">' +
    '<div class="color-bg4" style="position:absolute;width:144px;height:168px;top:0;left:0;background-color:#000"></div>' +
    '<div class="color-bg1" style="position:absolute;width:144px;height:102px;top:34px;left:0;border-radius:6px;background-color:#fff"></div>' +
    '<div class="color-bg2" style="position:absolute;width:142px;height:100px;top:35px;left:1px;border-radius:6px;background-color:#000"></div>' +
    '<div class="color-bg3" style="position:absolute;width:140px;height:98px;top:36px;left:2px;border-radius:6px;background-color:#fff"></div>' +
    '<div class="color-t3" style="color:#fff;font-size:68px;position:absolute;top:50px;left:3px;letter-spacing:-4px;font-family:DS-Digital,monospace"><span style="display:inline-block;width:30px;margin-right:1px;text-align:right">8</span><span style="display:inline-block;width:30px;margin-right:1px;text-align:right">8</span><span style="display:inline-block;width:11px;margin-right:1px;text-align:right">:</span><span style="display:inline-block;width:30px;margin-right:1px;text-align:right">8</span><span style="display:inline-block;width:30px;margin-right:1px;text-align:right">8</span></div>' +
    '<div class="color-t4" style="color:#000;font-size:68px;position:absolute;top:50px;left:3px;letter-spacing:-4px;font-family:DS-Digital,monospace"><span style="display:inline-block;width:30px;margin-right:1px;text-align:right">1</span><span style="display:inline-block;width:30px;margin-right:1px;text-align:right">2</span><span style="display:inline-block;width:11px;margin-right:1px;text-align:right">:</span><span style="display:inline-block;width:30px;margin-right:1px;text-align:right">3</span><span style="display:inline-block;width:30px;margin-right:1px;text-align:right">4</span></div>' +
    '<div class="color-t2" style="color:#000;font-size:11px;position:absolute;top:64px;left:15px;font-family:sans-serif">24H</div>' +
    '<div class="color-t1" style="color:#000;font-size:21px;position:absolute;top:54px;right:5px;font-family:DS-Digital,monospace">WED 22</div>' +
    '<div class="color-d1" style="height:1px;font-size:0;border-top:solid 1px #fff;position:absolute;top:19px;right:0;width:144px"></div>' +
    '<div class="color-d2" style="height:1px;font-size:0;border-top:solid 1px #fff;position:absolute;top:150px;right:0;width:144px"></div>' +
    '<div class="color-d9" style="font-family:sans-serif;font-size:14px;text-align:center;position:absolute;top:0;left:0;width:144px;color:#fff">pebble smartwatch</div>' +
    '<div style="font-family:monospace;font-size:9px;position:absolute;left:4px;top:22px;color:#fff"><span class="color-d8" style="position:relative;top:-1px">&#9664;</span> <span class="color-d7">LIGHT</span></div>' +
    '<div style="font-family:monospace;font-size:9px;position:absolute;right:4px;top:22px;color:#fff"><span class="color-d7">PREV</span> <span class="color-d8" style="position:relative;top:-1px">&#9654;</span></div>' +
    '<div style="font-family:monospace;font-size:9px;position:absolute;right:4px;bottom:18px;color:#fff"><span class="color-d7">NEXT</span> <span class="color-d8" style="position:relative;top:-1px">&#9654;</span></div>' +
    '<div class="color-bi1" data-preview-prop="borderColor" style="position:absolute;top:43px;right:6px;width:16px;height:9px;border:solid 1px #000"><div class="color-bi1" data-preview-prop="backgroundColor" style="position:relative;top:1px;background:#000;width:10px;height:5px;left:1px"></div></div>' +
    '<div class="color-bi1" data-preview-prop="color" style="font-family:monospace;font-size:10px;position:absolute;top:41px;right:23px;color:#000">80%</div>' +
    '<svg style="position:absolute;left:8px;top:42px" width="10" height="10">' +
    '<ellipse class="color-bl1" cx="5" cy="5" rx="5" ry="5" fill="#00009C"/>' +
    '<line class="color-bl2" x1="2" y1="2" x2="8" y2="8" stroke="#D3D3D3"/>' +
    '<line class="color-bl2" x1="2" y1="8" x2="8" y2="2" stroke="#D3D3D3"/>' +
    '<line class="color-bl2" x1="5" y1="1" x2="5" y2="8" stroke="#D3D3D3"/>' +
    '<line class="color-bl2" x1="8" y1="2" x2="8" y2="8" stroke="#D3D3D3"/>' +
    '</svg>' +
    '<div class="color-d6" style="position:absolute;top:152px;left:10px;color:#fff;font-size:10px;font-family:sans-serif">WATER</div>' +
    '<div class="color-d6" style="position:absolute;top:152px;left:96px;color:#fff;font-size:10px;font-family:sans-serif">RESIST</div>' +
    '<svg style="position:absolute;top:147px;left:50px" width="42" height="16">' +
    '<path class="color-d3 color-d4" d="m1.2099,3.12781l1.50781,-2.13223l36.27459,0.00001l1.5069,2.13222l-0.0065,7.83829l-4.49977,4.15946l-30.26651,0l-4.51749,-4.15946l0.00097,-7.83829z" stroke="#fff" fill="#000"/>' +
    '</svg>' +
    '<svg style="position:absolute;top:146px;left:59px" width="36" height="21" preserveAspectRatio="slice" viewBox="0 0 52.5 20">' +
    '<path class="color-d5" d="m 4.8975614,16.25 -2.2027547,0 -0.774796,-4.6875 C 1.4938729,8.9843754 1.1390242,5.6093754 1.1314578,4.0625004 L 1.1177008,1.25 l 2.5,0 2.4999997,0 0.06441,4.6875004 L 6.2465205,10.625 8.1908035,5.9375004 10.135085,1.25 l 1.741308,0 1.741307,0 0.121474,4.6875004 L 13.860649,10.625 15.783794,5.9375004 17.70694,1.25 l 1.70538,0 c 0.937959,0 1.70538,0.2311965 1.70538,0.5137699 0,0.2825735 -1.203065,3.6575735 -2.673479,7.4999995 L 15.770744,16.25 l -2.326523,0 -2.326521,0 L 11.05329,11.5625 10.98888,6.8750004 9.0445985,11.5625 7.1003165,16.25 l -2.202754,0 z m 17.3708196,0 -2.59932,0 0.72432,-3.75 c 0.398375,-2.0625 1.056816,-5.4673386 1.463201,-7.5663076 l 0.738883,-3.8163078 5.071311,0.3788078 5.071313,0.3788076 1.26374,2.0175964 1.26374,2.017597 -1.009793,2.357403 C 33.700391,9.5641684 33.369876,11.890625 33.5213,13.4375 l 0.275314,2.8125 -2.443668,0 -2.443667,0 -0.686655,-2.62577 -0.686654,-2.625769 -1.334135,0.824541 C 25.468061,12.276499 24.8677,13.458095 24.8677,14.448771 l 0,1.801229 -2.599319,0 z m 5.724319,-8.7499996 1.875,0 0,-1.25 0,-1.25 -1.875,0 -1.875,0 0,1.25 0,1.25 1.875,0 z" fill="#fff"/' +
    '</svg>' +
    '</div></div>',

  style: '@font-face{font-family:\'DS-Digital\';src:url(data:font/ttf;base64,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) format(\'truetype\')}[data-preview-root]{display:block;margin:0 auto}',

  manipulator: { get: function() {}, set: function() {} },

  defaults: {
    defaultValue: ''
  }
};
