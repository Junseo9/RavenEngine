# Source from the repository root: source scripts/macos-env.sh
# Use the SDK matching the active Xcode compiler and locate Homebrew's Vulkan
# validation library when the Vulkan loader opens it by filename.
if [ "$(uname -s)" != "Darwin" ]; then
    printf '%s\n' 'This environment is for macOS.' >&2
    return 1
fi

raven_macos_sdk="$(xcrun --sdk macosx --show-sdk-path)" || return 1
raven_validation_prefix="$(brew --prefix vulkan-validationlayers)" || return 1

export SDKROOT="$raven_macos_sdk"
export VK_LAYER_PATH="$raven_validation_prefix/share/vulkan/explicit_layer.d${VK_LAYER_PATH:+:$VK_LAYER_PATH}"
export DYLD_LIBRARY_PATH="$raven_validation_prefix/lib${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}"

unset raven_macos_sdk raven_validation_prefix
