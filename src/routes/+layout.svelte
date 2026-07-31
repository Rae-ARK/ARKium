<script lang="ts">
	import { onMount } from 'svelte';
	import AppShell from '$lib/components/layout/AppShell.svelte';
	import { libraryService } from '$lib/services/library';
	import { library } from '$lib/stores/library.svelte';

	let { children } = $props();

	onMount(async () => {
		try {
			const items = await libraryService.load();
			library.set(items);
		} catch (err) {
			console.error(err);
			library.setError('Could not load your library.');
		}
	});
</script>

<AppShell>
	{@render children()}
</AppShell>
