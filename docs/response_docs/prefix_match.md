# Location path must a prefix of uri
    so it must be fully contained at the beginning of uri

## 1
```
uri: /images
locations: /images/png
            /img
            /imag
            /imagesfggf

```
## 2

uri: /images/png
locations: /images
            /imagesfdfd
            /images/



