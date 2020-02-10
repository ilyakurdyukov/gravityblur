# Gravity Blur

This is an image smoothing algorithm inspired by [JPEG Quant Smooth](https://github.com/ilyakurdyukov/jpeg-quantsmooth). This is called *gravity blur* because the pixel values gravitate towards each other, the force of this *gravity* is inversely proportional to the distance between them.

This algorithm preserves sharp edges if the distance between pixel values on the sides of the edges is greater than *range*. With more iterations, it is not only smoothes the image, but can also make it look like CG art or poster.

The original project page is [here](https://github.com/ilyakurdyukov/gravityblur).

## WebAssembly

Web version available [here](https://ilyakurdyukov.github.io/gravityblur/).
Images are processed locally on your computer.

- Click the "Load" button or drag-n-drop image into the browser window. After processing is complete, you can save the result by clicking the "Save" button. You can edit the *filename* field before saving to change the image format (jpeg or png, JPEG images are saved at 100% quality). The *options* field is passed to the *wasm* code when you initiate a processing by loading a file.

## Usage

`gravblur [options] input.[png|jpg|jpeg] output.[png|jpg|jpeg]`

## Options

`-r, --range f` Gravity range (0-100)  
`-s, --sharp f` Sharpness (0-100)  
`-n, --niter n` Number of iterations (default is 3)  
`-R, --rgb` Process in RGB (default)  
`-Y, --yuv` Process in YUV  
`-S, --separate` Separate color components  
`-a, --alpha` Use alpha channel  
`-i, --info n` Print gravblur debug output: 0 - silent, 3 - all (default)  

## Examples

- Images 3x zoomed.

![](https://ilyakurdyukov.github.io/gravityblur/images/lena_orig.png)
![](https://ilyakurdyukov.github.io/gravityblur/images/lena_new.png)

## Buliding

If your system have *libjpeg* and *libpng* development packages installed, just type `make`.
Or you can specify their location with `LIBS` variable to `make`.

