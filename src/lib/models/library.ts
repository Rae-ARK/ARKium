export interface LibraryEntry {
	id: string;
	title: string;
	author?: string;
	path: string;
	lastRead?: number;
	progress?: number;
	addedAt: string;
}

export type Library = LibraryEntry[];
