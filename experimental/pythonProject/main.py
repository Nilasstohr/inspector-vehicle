import cv2
import numpy as np

# Settings
MARKER_ID = 0                  # Must match target_marker_id_ in DockingNode
MARKER_SIZE_PX = 400           # Pixels (will be scaled to physical size when printing)
PHYSICAL_SIZE_CM = 10          # 10 x 10 cm
DICTIONARY = cv2.aruco.DICT_4X4_50  # Must match aruco_dict_ in DockingNode

# Generate marker
aruco_dict = cv2.aruco.getPredefinedDictionary(DICTIONARY)
marker_image = np.zeros((MARKER_SIZE_PX, MARKER_SIZE_PX), dtype=np.uint8)
cv2.aruco.generateImageMarker(aruco_dict, MARKER_ID, MARKER_SIZE_PX, marker_image, 1)

# Add white border (recommended for detection)
border = 40
marker_with_border = cv2.copyMakeBorder(
    marker_image, border, border, border, border,
    cv2.BORDER_CONSTANT, value=255
)

output_file = f"aruco_marker_id{MARKER_ID}_{PHYSICAL_SIZE_CM}cm.png"
cv2.imwrite(output_file, marker_with_border)
print(f"Saved: {output_file}")
print(f"Print this image at exactly {PHYSICAL_SIZE_CM}x{PHYSICAL_SIZE_CM} cm")
print(f"Marker ID: {MARKER_ID}, Dictionary: DICT_4X4_50")