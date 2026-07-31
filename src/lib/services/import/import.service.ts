import { fileProvider } from '$lib/services/file';
import { libraryService } from '$lib/services/library';
import { library } from '$lib/stores/library.svelte';
import type { LibraryEntry } from '$lib/models';
import { extractMetadataFromFilename, isTxtFile } from './metadata';

export class ImportService {
	/**
	 * Opens the folder picker, discovers TXT files inside the chosen folder,
	 * and adds any not already in the library. Existing entries are preserved.
	 */
	async chooseLibraryFolder(): Promise<LibraryEntry[]> {
		const folder = await fileProvider.pickFolder();
		const files = await fileProvider.listFiles(folder.id);

		const txtFiles = files.filter((file) => file.type === 'file' && isTxtFile(file.name));

		const existingPaths = new Set(library.items.map((item) => item.path));

		const newEntries: LibraryEntry[] = txtFiles
			.filter((file) => !existingPaths.has(file.path))
			.map((file) => {
				const metadata = extractMetadataFromFilename(file.name);
				return {
					id: crypto.randomUUID(),
					title: metadata.title,
					author: metadata.author,
					path: file.path,
					progress: 0,
					addedAt: new Date().toISOString()
				};
			});

		if (newEntries.length === 0) {
			return [];
		}

		for (const entry of newEntries) {
			library.add(entry);
		}

		await libraryService.save(library.items);

		return newEntries;
	}
}

export const importService = new ImportService();
