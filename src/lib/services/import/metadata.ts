export interface ExtractedMetadata {
	title: string;
	author?: string;
}

const TXT_EXTENSION = /\.txt$/i;

/**
 * Extracts basic metadata from a TXT filename.
 * Supports the common "Title - Author.txt" convention; otherwise the
 * whole filename (minus extension) becomes the title.
 * EPUB/PDF metadata parsing is out of scope until those formats land.
 */
export function extractMetadataFromFilename(filename: string): ExtractedMetadata {
	const withoutExtension = filename.replace(TXT_EXTENSION, '').trim();
	const separatorIndex = withoutExtension.indexOf(' - ');

	if (separatorIndex === -1) {
		return { title: withoutExtension || filename };
	}

	const title = withoutExtension.slice(0, separatorIndex).trim();
	const author = withoutExtension.slice(separatorIndex + 3).trim();

	return {
		title: title || withoutExtension,
		author: author || undefined
	};
}

export function isTxtFile(filename: string): boolean {
	return TXT_EXTENSION.test(filename);
}
