#pragma once

#if !defined(_WIN32) && !defined(_WIN64)
#define CVR_API __attribute__((visibility("default")))
#include <stddef.h>
#else
#ifdef CVR_EXPORTS
#define CVR_API __declspec(dllexport)
#else
#define CVR_API __declspec(dllimport)
#endif
#include <windows.h>
#endif

#include "DynamsoftCore.h"
#include "DynamsoftLabelRecognizer.h"
#include "DynamsoftBarcodeReader.h"
#include "DynamsoftDocumentNormalizer.h"
#include "DynamsoftCodeParser.h"
#include "DynamsoftLicense.h"
#include "DynamsoftUtility.h"

#define DCV_VERSION                  "2.0.20.0925"

/**Enumeration section*/

/**
* @enum CaptureState
*
* Describes the state of data capturing.
*/
typedef enum CaptureState
{
	/**The data capturing is started.*/
	CS_STARTED,

	/**The data capturing is stopped.*/
	CS_STOPPED
} CaptureState;

/**
* @enum ImageSourceState
*
* Describes the state of ImageSourceAdapter.
*/
typedef enum ImageSourceState
{
	/**The buffer of ImageSourceAdapter is temporarily empty.*/
	ISS_BUFFER_EMPTY,

	/**The source of ImageSourceAdapter is empty.*/
	ISS_EXHAUSTED
} ImageSourceState;

/**Structures section*/
#pragma pack(push)
#pragma pack(1)

/**
* The `SimplifiedCaptureVisionSettings` struct contains settings for capturing and recognizing images with the `CCaptureVisionRouter` class.
*/
typedef struct tagSimplifiedCaptureVisionSettings 
{
	/**
	 * Specifies the type(s) of CapturedItem(s) that will be captured.
	 *
	 * @values The value should be a bitwise OR combination of one or more of the following constants:
	 *         `DCV_CAPTURED_ITEM_TYPE_BARCODE`
	 *         `DCV_CAPTURED_ITEM_TYPE_LABEL`
	 */
	int capturedResultItemTypes;

	/**
	 * Specifies the region of interest (ROI) where the image capture and recognition will take place.
	 */
	CQuadrilateral roi;

	/**
	 * Specifies whether the ROI is measured in pixels or as a percentage of the image size.
	 *
	 * @values 0 if the ROI is measured in pixels.
	 *         1 if the ROI is measured as a percentage of the image size.
	 */
	int roiMeasuredInPercentage;

	/**
	 * Specifies the maximum number of parallel tasks that can be used for image capture and recognition.
	 */
	int maxParallelTasks;

	/**
	 * Specifies the maximum time (in milliseconds) allowed for image capture and recognition.
	 */
	int timeout;

	/**
	 * Specifies the settings for barcode recognition.
	 */
	SimplifiedBarcodeReaderSettings barcodeSettings;

	/**
	 * Specifies the settings for label recognition.
	 */
	SimplifiedLabelRecognizerSettings labelSettings;
	/**
	 * @brief Minimum time interval (in milliseconds) allowed between consecutive image captures.
	 *
	 * This property represents the minimum time interval (in milliseconds) that must
	 * elapse before the next image capture operation can be initiated.
	 * Setting a larger value for this property will introduce a delay between image
	 * captures, while setting a smaller value allows for more frequent captures. It
	 * can be used to reduce the computational frequency, which can effectively lower
	 * energy consumption.
	 *
	 * @note The actual time interval between captures may be longer than the specified
	 *       minimum interval due to various factors, such as image processing time and
	 *       hardware limitations.
	 *
	 */
	int minImageCaptureInterval;

	/**
	 * Reserved for future use.
	 */
	char reserved[2044];

} SimplifiedCaptureVisionSettings;

#pragma pack(pop)

/**C API section*/
#ifdef __cplusplus

using namespace dynamsoft::basic_structures;
using namespace dynamsoft::intermediate_results;

class CaptureVisionInner;

namespace dynamsoft
{
	namespace cvr
	{
		/**
		* The CCaptureStateListener class is an abstract class that defines a listener for capture state changes.
		*
		*/
		class CVR_API CCaptureStateListener
		{
		public:
			/**
			* Called when the capture state changes.
			*
			* @param [in] state The new capture state.
			*
			* @return This method does not return a value. It is a pure virtual method and must be implemented by a derived class.
			*
			*/
			virtual void OnCaptureStateChanged(CaptureState state) = 0;
		};

		/**
		* The `CImageSourceStateListener` class is an abstract class that defines a listener for image source state changes.
		*
		*/
		class CVR_API CImageSourceStateListener
		{
		public:
			/**
			* This method is called when the state of the image source changes.
			*
			* @param [in] state The new state of the image source.
			*
			*/
			virtual void OnImageSourceStateReceived(ImageSourceState state) = 0;
		};

		/**
		* The `CPresetTemplate` class defines all preset template names of Capture Vision SDK.
		*
		*/
		class CPresetTemplate 
		{
		public:
			CVR_API static const char*  PT_DEFAULT;							//"Default", compatible with "default": DBR + DLR + DDN-Normalize
			CVR_API static const char*	PT_READ_BARCODES;					//"ReadBarcodes_Default", compatible with "read-barcodes"
			CVR_API static const char*	PT_RECOGNIZE_TEXT_LINES;			//"RecognizeTextLines_Default", compatible with "recognize-textlines"
			CVR_API static const char*	PT_DETECT_DOCUMENT_BOUNDARIES;		//"DetectDocumentBoundaries_Default", compatible with "detect-document-boundaries"
			CVR_API static const char*	PT_DETECT_AND_NORMALIZE_DOCUMENT;   //"DetectAndNormalizeDocument_Default", compatible with "detect-and-normalize-document"
			CVR_API static const char*	PT_NORMALIZE_DOCUMENT;				//"NormalizeDocument_Default", compatible with "normalize-document"

			/**
			 * @brief Represents a barcode reading mode where speed is prioritized.
			 *
			 * In this mode, the barcode reader will optimize for faster barcode detection
			 * and decoding, sacrificing some level of accuracy and read rate. It is suitable
			 * for situations where a quick response time is more important than perfect
			 * barcode recognition.
			 */
			CVR_API static const char* PT_READ_BARCODES_SPEED_FIRST; //"ReadBarcodes_SpeedFirst";

			/**
			 * @brief Represents a barcode reading mode where barcode read rate is prioritized.
			 *
			 * In this mode, the barcode reader will optimize for higher barcode read rates,
			 * even if it may sometimes result in reduced accuracy and speed. It is suitable for
			 * scenarios where maximizing the number of successfully read barcodes is critical.
			 */
			CVR_API static const char* PT_READ_BARCODES_READ_RATE_FIRST; //"ReadBarcodes_ReadRateFirst";

			/**
			* @brief Represents a barcode reading mode for single barcode code detection.
			*
			* In this mode, the barcode reader will focus on detecting and decoding a single
			* barcode code, ignoring any additional codes in the same image. It is efficient
			* when the target image has only one barcode.
			*/
			CVR_API static const char* PT_READ_SINGLE_BARCODE; //"ReadSingleBarcode";

			/**
			* @brief Represents a text recognition mode focused on recognizing numbers.
			*/
			CVR_API static const char* PT_RECOGNIZE_NUMBERS; //"RecognizeNumbers";

			/**
			 * @brief Represents a text recognition mode focused on recognizing alphabetic characters (letters).
			 *
			 */
			CVR_API static const char* PT_RECOGNIZE_LETTERS; //"RecognizeLetters";

			/**
			 * @brief Represents a text recognition mode that combines numbers and alphabetic characters (letters) recognition.
			 */
			CVR_API static const char* PT_RECOGNIZE_NUMBERS_AND_LETTERS; //"RecognizeNumbersAndLetters";

			/**
			 * @brief Represents a text recognition mode that combines numbers and uppercase letters recognition.
			 */
			CVR_API static const char* PT_RECOGNIZE_NUMBERS_AND_UPPERCASE_LETTERS; //"RecognizeNumbersAndUppercaseLetters";

			/**
			 * @brief Represents a text recognition mode focused on recognizing uppercase letters.
			 */
			CVR_API static const char* PT_RECOGNIZE_UPPERCASE_LETTERS; //"RecognizeUppercaseLetters";
		};

		/**
		* The CCaptureVisionRouter class is what a user uses to interact with image-processing and semantic-processing products in their applications.
		* It accepts an image source and returns processing results which may contain Final results or Intermediate Results.
		*/
		class CVR_API CCaptureVisionRouter
		{
		protected:
			CaptureVisionInner * m_DCVInner;

		public:
			/**
			* Default constructor of `CCaptureVisionRouter` object.
			*/
			CCaptureVisionRouter();

			/**
			* Destructor of `CCaptureVisionRouter` object.
			*/
			~CCaptureVisionRouter();

			/**
			* Loads and initializes a template from a string.
			*
			* @param [in] content The string containing the template.
			* @param [in] errorMsgBuffer A buffer for error messages.
			* @param [in] errorMsgBufferLen The length of the error message buffer.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int InitSettings(const char* content, char errorMsgBuffer[] = NULL, const int errorMsgBufferLen = 0);

			/**
			* Loads and initializes a template from a file.
			*
			* @param [in] filePath The path to the file containing the template.
			* @param [in] errorMsgBuffer A buffer for error messages.
			* @param [in] errorMsgBufferLen The length of the error message buffer.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int InitSettingsFromFile(const char* filePath, char errorMsgBuffer[] = NULL, const int errorMsgBufferLen = 0);

			/**
			* Exports a specific template to a string.
			*
			* @param [in] templateName The name of the template to export.
			* @param [out] pErrorCode An error code.
			*
			* @return Returns a string containing the exported template. The string is allocated by the SDK and must be freed by calling `FreeString`.
			*
			*/
			char* OutputSettings(const char* templateName, int* pErrorCode = NULL);

			/**
			* Exports a specific template to a file.
			*
			* @param [in] templateName The name of the template to export.
			* @param [in] filePath The path to the output file.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int OutputSettingsToFile(const char* templateName, const char* filePath);

			/**
			* Retrieves a simplified version of the capture settings for a specific template.
			*
			* @param [in] templateName The name of the template.
			* @param [out] settings A pointer to a `SimplifiedCaptureVisionSettings` object.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int GetSimplifiedSettings(const char* templateName, SimplifiedCaptureVisionSettings* settings);
			
			/**
			* Updates a template with simplified capture settings.
			*
			* @param [in] templateName The name of the template to update.
			* @param [in] settings A pointer to a `SimplifiedCaptureVisionSettings` object.
			* @param [in] errorMsgBuffer A buffer for error messages.
			* @param [in] errorMsgBufferLen The length of the error message buffer.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int UpdateSettings(const char* templateName, const SimplifiedCaptureVisionSettings* settings, char errorMsgBuffer[] = NULL, const int errorMsgBufferLen = 0);
			
			/**
			* Resets all templates to factory settings.
			*
			*/
			int ResetSettings();

			/**
			* Process an image or file to derive important information. It can optionally use a specified template for the capture.
			*
			* @param [in] filePath Specifies the path of the file to process.
			* @param [in] templateName Specifies the template to use for capturing. Default value is an empty string which means the factory default template.
			*
			* @return Returns a pointer to a `CCapturedResult` object containing the captured result.
			*
			*/
			CCapturedResult* Capture(const char* filePath, const char* templateName = "");

			/**
			* Process an image or file to derive important information. It can optionally use a specified template for the capture.
			*
			* @param [in] fileBytes Specifies the memory location containing the image to be processed.
			* @param [in] fileSize Specifies the size of the image in bytes.
			* @param [in] templateName Specifies the template to use for capturing. Default value is an empty string which means the factory default template.
			*
			* @return Returns a pointer to a `CCapturedResult` object containing the captured result.
			*
			*/
			CCapturedResult* Capture(const unsigned char* fileBytes, int fileSize, const char* templateName = "");

			/**
			* Process an image or file to derive important information. It can optionally use a specified template for the capture.
			*
			* @param [in] pImageData Specifies the image data to process.
			* @param [in] templateName Specifies the template to use for capturing. Default value is an empty string which means the factory default template.
			*
			* @return Returns a pointer to a `CCapturedResult` object containing the captured result.
			*
			*/
			CCapturedResult* Capture(const CImageData* pImageData, const char* templateName = "");

			/**
			* Sets an image source to provide images for consecutive processing.
			*
			* @param [in] pAdaptor Specifies an object which has implemented the Image Source Adapter Interface.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int SetInput(CImageSourceAdapter* pAdapter);

			/**
			* Gets the attached image source adapter object of the capture vision router.
			*
			* @return Returns the attached image source adapter object of the capture vision router.
			*
			*/
			CImageSourceAdapter* GetInput();

			/**
			* Adds an object that listens to state changes of the image source.
			*
			* @param [in] listener Specifies a listening object of the type CImageSourceStateListener to be added.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int AddImageSourceStateListener(CImageSourceStateListener* listener);

			/**
			* Removes an object which listens to state changes of the image source.
			*
			* @param [in] listener Specifies a listening object of the type CImageSourceStateListener to be removed.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int RemoveImageSourceStateListener(CImageSourceStateListener* listener);

			/**
			* Adds an object as the receiver of captured results.
			*
			* @param [in] receiver Specifies a receiver object of the type CCapturedResultReceiver to be added.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int AddResultReceiver(CCapturedResultReceiver* receiver);

			/**
			* Removes an object which was added as a receiver of captured results.
			*
			* @param [in] receiver Specifies a receiver object of the type CCapturedResultReceiver to be removed.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int RemoveResultReceiver(CCapturedResultReceiver* receiver);

			/**
			* Adds an object as the filter of captured results.
			*
			* @param [in] filter Specifies a filter object of the type CCapturedResultFilter to be added.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int AddResultFilter(CCapturedResultFilter* filter);

			/**
			* Removes an object which was added as a filter of captured results.
			*
			* @param [in] filter Specifies a filter object of the type CCapturedResultFilter to be removed.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int RemoveResultFilter(CCapturedResultFilter* filter);

			/**
			* Adds an object that listens to the state changes of the capture process.
			*
			* @param [in] listener Specifies a listening object of the type CCaptureStateListener to be added.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int AddCaptureStateListener(CCaptureStateListener* listener);

			/**
			* Removes an object which listens to the state changes of the capture process.
			*
			* @param [in] listener Specifies a listening object of the type CCaptureStateListener to be removed.
			*
			* @return Returns an error code. Zero indicates success.
			*
			*/
			int RemoveCaptureStateListener(CCaptureStateListener* listener);

			/**
			* Starts to process images consecutively.
			*
			* @param [in] templateName Specifies a template to use for capturing. If not specified, an empty string is used which means the factory default template.
			*
			* @param [in] waitForThreadExit Indicates whether to wait for the capture process to complete before returning. The default value is false.
			*
			* @param [out] errorMsgBuffer Stores any error messages generated during the capturing process. If no buffer is provided, the error messages will not be output.
			*
			* @param [in] errorMsgBufferLen Specifies the length of the provided error message buffer. If no buffer is provided, this parameter is ignored.
			*
			* @return The function returns an integer value representing the success or failure of the capturing process. A value of 0 indicates success, while a non-zero value indicates failure. If an error message buffer is provided, any error messages generated during the capturing process will be stored in the buffer.
			*
			*/
			int StartCapturing(const char* templateName = "", bool waitForThreadExit = false, char errorMsgBuffer[] = NULL, const int errorMsgBufferLen = 0);

			/**
			* Stops the multiple-file processing.
			*
			* @param [in] waitForRemainingTasks Indicates whether to wait for the remaining tasks to complete before returning. The default value is true.
			*
			* @param [in] waitForThreadExit Indicates whether to wait for the capture process to complete before returning. The default value is false.
			*
			* @return None.
			*
			*/
			void StopCapturing(bool waitForRemainingTasks = true, bool waitForThreadExit = false);

			/**
			* Frees the memory allocated for a string.
			*
			* @param [in] content The string whose memory needs to be freed.
			*
			*/
			static void FreeString(char* content);

			/**
			* Returns an `CIntermediateResultManager` object.
			*
			* @return Returns a pointer to the `CIntermediateResultManager` object.
			*
			*/
			CIntermediateResultManager* GetIntermediateResultManager();

		private:
			CCaptureVisionRouter(const CCaptureVisionRouter& r);
			CCaptureVisionRouter& operator=(const CCaptureVisionRouter& r);
		};

		/**
		 * The CCaptureVisionRouterModule class defines general functions in the capture vision router module.
		 */
		class CVR_API CCaptureVisionRouterModule
		{
		public:
			/**
			 * Returns the version of the capture vision router module.
			 *
			 * @return Returns a const char pointer representing the version of the capture vision router module.
			 */
			static const char* GetVersion();
		};
	}
}
#endif