var b;b||(b=typeof Module !== 'undefined' ? Module : {});var f={},h;for(h in b)b.hasOwnProperty(h)&&(f[h]=b[h]);var k=!1,l=!1,m=!1,aa=!1,n=!1;k="object"===typeof window;l="function"===typeof importScripts;m=(aa="object"===typeof process&&"object"===typeof process.versions&&"string"===typeof process.versions.node)&&!k&&!l;n=!k&&!m&&!l;var p="",q,u,x,y;
if(m)p=__dirname+"/",q=function(a,c){x||(x=require("fs"));y||(y=require("path"));a=y.normalize(a);return x.readFileSync(a,c?null:"utf8")},u=function(a){a=q(a,!0);a.buffer||(a=new Uint8Array(a));a.buffer||z("Assertion failed: undefined");return a},1<process.argv.length&&process.argv[1].replace(/\\/g,"/"),process.argv.slice(2),"undefined"!==typeof module&&(module.exports=b),process.on("uncaughtException",function(a){throw a;}),process.on("unhandledRejection",z),b.inspect=function(){return"[Emscripten Module object]"};
else if(n)"undefined"!=typeof read&&(q=function(a){return read(a)}),u=function(a){if("function"===typeof readbuffer)return new Uint8Array(readbuffer(a));a=read(a,"binary");"object"===typeof a||z("Assertion failed: undefined");return a},"undefined"!==typeof print&&("undefined"===typeof console&&(console={}),console.log=print,console.warn=console.error="undefined"!==typeof printErr?printErr:print);else if(k||l)l?p=self.location.href:document.currentScript&&(p=document.currentScript.src),p=0!==p.indexOf("blob:")?
p.substr(0,p.lastIndexOf("/")+1):"",q=function(a){var c=new XMLHttpRequest;c.open("GET",a,!1);c.send(null);return c.responseText},l&&(u=function(a){var c=new XMLHttpRequest;c.open("GET",a,!1);c.responseType="arraybuffer";c.send(null);return new Uint8Array(c.response)});var A=b.print||console.log.bind(console),B=b.printErr||console.warn.bind(console);for(h in f)f.hasOwnProperty(h)&&(b[h]=f[h]);f=null;var C;b.wasmBinary&&(C=b.wasmBinary);"object"!==typeof WebAssembly&&B("no native wasm support detected");
var D,ba=new WebAssembly.Table({initial:5,maximum:5,element:"anyfunc"}),E=!1,H="undefined"!==typeof TextDecoder?new TextDecoder("utf8"):void 0;"undefined"!==typeof TextDecoder&&new TextDecoder("utf-16le");function I(a){0<a%65536&&(a+=65536-a%65536);return a}var buffer,J,K,L;
function R(a){buffer=a;b.HEAP8=J=new Int8Array(a);b.HEAP16=new Int16Array(a);b.HEAP32=L=new Int32Array(a);b.HEAPU8=K=new Uint8Array(a);b.HEAPU16=new Uint16Array(a);b.HEAPU32=new Uint32Array(a);b.HEAPF32=new Float32Array(a);b.HEAPF64=new Float64Array(a)}var S=b.TOTAL_MEMORY||16777216;b.wasmMemory?D=b.wasmMemory:D=new WebAssembly.Memory({initial:S/65536});D&&(buffer=D.buffer);S=buffer.byteLength;R(buffer);L[948]=5246832;
function T(a){for(;0<a.length;){var c=a.shift();if("function"==typeof c)c();else{var d=c.l;"number"===typeof d?void 0===c.j?b.dynCall_v(d):b.dynCall_vi(d,c.j):d(void 0===c.j?null:c.j)}}}var ca=[],da=[],ea=[],fa=[];function la(){var a=b.preRun.shift();ca.unshift(a)}var ma=Math.ceil,na=Math.floor,U=0,V=null,W=null;b.preloadedImages={};b.preloadedAudios={};
function z(a){if(b.onAbort)b.onAbort(a);A(a);B(a);E=!0;throw new WebAssembly.RuntimeError("abort("+a+"). Build with -s ASSERTIONS=1 for more info.");}function oa(){var a=X;return String.prototype.startsWith?a.startsWith("data:application/octet-stream;base64,"):0===a.indexOf("data:application/octet-stream;base64,")}var X="gravityblur.wasm";if(!oa()){var pa=X;X=b.locateFile?b.locateFile(pa,p):p+pa}
function qa(){try{if(C)return new Uint8Array(C);if(u)return u(X);throw"both async and sync fetching of the wasm failed";}catch(a){z(a)}}function ra(){return C||!k&&!l||"function"!==typeof fetch?new Promise(function(a){a(qa())}):fetch(X,{credentials:"same-origin"}).then(function(a){if(!a.ok)throw"failed to load wasm binary file at '"+X+"'";return a.arrayBuffer()}).catch(function(){return qa()})}da.push({l:function(){sa()}});
var ta=[null,[],[]],ua={c:function(a,c,d){K.set(K.subarray(c,c+d),a)},d:function(a){if(2147418112<a)return!1;for(var c=Math.max(J.length,16777216);c<a;)536870912>=c?c=I(2*c):c=Math.min(I((3*c+2147483648)/4),2147418112);a:{try{D.grow(c-buffer.byteLength+65535>>16);R(D.buffer);var d=1;break a}catch(v){}d=void 0}return d?!0:!1},e:function(a,c,d,v){try{for(var F=0,G=0;G<d;G++){for(var wa=L[c+8*G>>2],ha=L[c+(8*G+4)>>2],M=0;M<ha;M++){var N=K[wa+M],O=ta[a];if(0===N||10===N){var xa=1===a?A:B;for(var r=O,
g=0,t=g+void 0,w=g;r[w]&&!(w>=t);)++w;if(16<w-g&&r.subarray&&H)var ia=H.decode(r.subarray(g,w));else{for(t="";g<w;){var e=r[g++];if(e&128){var P=r[g++]&63;if(192==(e&224))t+=String.fromCharCode((e&31)<<6|P);else{var ja=r[g++]&63;e=224==(e&240)?(e&15)<<12|P<<6|ja:(e&7)<<18|P<<12|ja<<6|r[g++]&63;if(65536>e)t+=String.fromCharCode(e);else{var ka=e-65536;t+=String.fromCharCode(55296|ka>>10,56320|ka&1023)}}}else t+=String.fromCharCode(e)}ia=t}xa(ia);O.length=0}else O.push(N)}F+=ha}L[v>>2]=F;return 0}catch(Q){return"undefined"!==
typeof FS&&Q instanceof FS.m||z(Q),Q.o}},b:function(a){var c=Date.now();L[a>>2]=c/1E3|0;L[a+4>>2]=c%1E3*1E3|0;return 0},memory:D,a:function(a){a=+a;return 0<=a?+na(a+.5):+ma(a-.5)},table:ba},va=function(){function a(a){b.asm=a.exports;U--;b.monitorRunDependencies&&b.monitorRunDependencies(U);0==U&&(null!==V&&(clearInterval(V),V=null),W&&(a=W,W=null,a()))}function c(c){a(c.instance)}function d(a){return ra().then(function(a){return WebAssembly.instantiate(a,v)}).then(a,function(a){B("failed to asynchronously prepare wasm: "+
a);z(a)})}var v={env:ua,wasi_snapshot_preview1:ua};U++;b.monitorRunDependencies&&b.monitorRunDependencies(U);if(b.instantiateWasm)try{return b.instantiateWasm(v,a)}catch(F){return B("Module.instantiateWasm callback failed with error: "+F),!1}(function(){if(C||"function"!==typeof WebAssembly.instantiateStreaming||oa()||"function"!==typeof fetch)return d(c);fetch(X,{credentials:"same-origin"}).then(function(a){return WebAssembly.instantiateStreaming(a,v).then(c,function(a){B("wasm streaming compile failed: "+
a);B("falling back to ArrayBuffer instantiation");d(c)})})})();return{}}();b.asm=va;var sa=b.___wasm_call_ctors=function(){return b.asm.f.apply(null,arguments)};b._web_process=function(){return b.asm.g.apply(null,arguments)};b._malloc=function(){return b.asm.h.apply(null,arguments)};b._free=function(){return b.asm.i.apply(null,arguments)};b.asm=va;var Y;W=function ya(){Y||Z();Y||(W=ya)};
function Z(){function a(){if(!Y&&(Y=!0,!E)){T(da);T(ea);if(b.onRuntimeInitialized)b.onRuntimeInitialized();if(b.postRun)for("function"==typeof b.postRun&&(b.postRun=[b.postRun]);b.postRun.length;){var a=b.postRun.shift();fa.unshift(a)}T(fa)}}if(!(0<U)){if(b.preRun)for("function"==typeof b.preRun&&(b.preRun=[b.preRun]);b.preRun.length;)la();T(ca);0<U||(b.setStatus?(b.setStatus("Running..."),setTimeout(function(){setTimeout(function(){b.setStatus("")},1);a()},1)):a())}}b.run=Z;
if(b.preInit)for("function"==typeof b.preInit&&(b.preInit=[b.preInit]);0<b.preInit.length;)b.preInit.pop()();Z();