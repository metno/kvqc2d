#! /bin/bash

export DOCKER_BUILDKIT=1

registry="registry.met.no/met/obsklim/bakkeobservasjoner/data-og-kvalitet/kvalobs/kvbuild"
kvuser=kvalobs
kvuserid=5010
mode="test"
targets=kvqc2d
default_os=noble
kvcpp_tag=latest
os=noble
nocache=
build="true"
push="true"
VERSION="$(./debian-version.sh)"
tag="$VERSION"
BUILDDATE=$(date +'%Y%m%d')
KV_BUILD_DATE=${KV_BUILD_DATE:-}
tags=""
tag_counter=0

if [ -n "${KV_BUILD_DATE}" ]; then
  BUILDDATE=$KV_BUILD_DATE
fi


use() {

  usage="\
Usage: $0 [--help] [options]

This script build a kvqc2d container. 

If --staging or --prod is given it is copied to the 
container registry at $registry. 
If --test, the default, is used it will not be copied 
to the registry.


Options:
  --help        display this help and exit.
  --tag tagname tag the image with the name tagname, default $tag.
  --tag-and-latest tagname tag the image with the name tagname  and also create latest tag.
  --tag-with-build-date 
                Creates three tags: ${VERSION}, latest and a ${VERSION}-${BUILDDATE}.
                If the enviroment variable KV_BUILD_DATE is set use
                this as the build date. Format KV_BUILD_DATE YYYYMMDD.
  --staging     build and push to staging.
  --prod        build and push to prod.
  --test        only build. Default.
  --no-cache    Do not use the docker build cache.
  --kvcpp-tag tag the kvcpp image with the name tagname, default $kvcpp_tag.  
  --build-only  Stop after building.
  --push-only   Push a previous build to registry. Must use the same flags as when building.
  --print-version-tag 
                Print the version tag and exit.

  The following opptions is mutally exclusive: --tag, --tag-and-latest, --tag-with-build-date
  The following options is mutally exclusive: --build-only, --push-only
  The following options is mutally exclusive: --staging, --prod, --test
              
"
echo -e "$usage\n\n"

}


while test $# -ne 0; do
  case $1 in
    --tag) tag=$2; shift;;
    --tag-and-latest) 
        tag="$2"
        tags="latest"
        tag_counter=$((tag_counter + 1))
        shift
        ;;
    --tag-with-build-date)
        tag_counter=$((tag_counter + 1))
        tags="latest $VERSION-$BUILDDATE"
        ;;
    --help) 
        use
        exit 0
        ;;
    --kvcpp-tag) kvcpp_tag=$2; shift;;
    --staging) mode="staging";;
    --prod) mode="prod";;
    --test) mode="test";;
    --no-cache) nocache="--no-cache";;
    --build-only)
        push="false";;
    --push-only)
        build="false";;
    --print-version-tag)
        echo "$VERSION-$BUILDDATE"
        exit 0;;
    -*) use
      echo "Invalid option $1"
      exit 1;;  
    *) targets="$targets $1";;
  esac
  shift
done

if [ $tag_counter -gt 1 ]; then
  echo "Only one of --tag, --tag-and-latest or --tag-with-build-date can be used"
  exit 1
fi

echo "KVQC2_VERSION: $KVQC2_VERSION"
echo "tag: $tag"
echo "tags: $tags"
echo "kvcpp_tag: $kvcpp_tag"
echo "mode: $mode"
echo "os: $os"
echo "Build mode: $mode"
echo "targets: $targets"
echo "build: $build"
echo "push: $push"
echo "nocache: $nocache"


if [ "$mode" = "test" ]; then 
  kvuserid=$(id -u)
  registry="$os/"
elif  [ "$os" = "$default_os" ]; then
  registry="$registry/$mode/"
else
  registry="$registry/$mode-$os/"
fi


echo "registry: $registry"
echo "tag: $tag"

for target in $targets ; do
  if [ "$build" = "true" ]; then
    docker build $nocache \
      --build-arg "kvuser=$kvuser" --build-arg "kvuserid=$kvuserid" \
      --build-arg "BASE_IMAGE_TAG=${kvcpp_tag}" \
      --build-arg "REGISTRY=${registry}" \
      -f docker/${os}/${target}.dockerfile  --tag ${registry}${target}:$tag .
  
    for tagname in $tags; do
      echo "Tagging: ${registry}${target}:$tagname"
      docker tag "${registry}${target}:$tag" "${registry}${target}:$tagname"
    done
  fi

  if [ $mode != test ] && [ "$push" = "true" ]; then 
    # Before pushing add the tag to the list of tags
    tags="$tag $tags"

    for tagname in $tags; do
      echo "Pushing: ${registry}${target}:$tagname"
      docker push "${registry}${target}:$tagname"
    done
  fi
done


