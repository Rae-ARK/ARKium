<script lang="ts">
	let { children } = $props();
</script>

<section>
	{@render children()}
</section>

<style>
	section {
		background: var(--surface);
		border-radius: 12px;
		padding: 1rem;
	}
</style>
