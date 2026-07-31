import type { LibraryEntry } from '$lib/models';

export type LibraryStatus = 'loading' | 'ready' | 'error';

class LibraryStore {
	items = $state<LibraryEntry[]>([]);
	status = $state<LibraryStatus>('loading');
	error = $state<string | null>(null);

	set(items: LibraryEntry[]) {
		this.items = items;
		this.status = 'ready';
		this.error = null;
	}

	setError(message: string) {
		this.status = 'error';
		this.error = message;
	}

	add(item: LibraryEntry) {
		this.items = [...this.items, item];
	}

	remove(id: string) {
		this.items = this.items.filter((item) => item.id !== id);
	}

	clear() {
		this.items = [];
	}
}

export const library = new LibraryStore();
