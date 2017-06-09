/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rendering_layer.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stoupin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/07 11:30:00 by stoupin           #+#    #+#             */
/*   Updated: 2017/06/07 11:30:02 by stoupin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "wolf3d.h"

void				rendering_layer_init(t_rendering_layer *layer,
													char **file_names, int n)
{
	layer->n = 0;
	if (!(layer->bmp = load_bitmap(file_names, n)))
		exit(EXIT_FAILURE);
	if (!(layer->ij = (t_coord_i*)
							ft_memalloc(sizeof(t_coord_i) * WIDTH * HEIGHT)))
		exit(EXIT_FAILURE);
	if (!(layer->uv = (t_coord_f*)
							ft_memalloc(sizeof(t_coord_f) * WIDTH * HEIGHT)))
		exit(EXIT_FAILURE);
	if (!(layer->dist = (float*)ft_memalloc(sizeof(float) * WIDTH * HEIGHT)))
		exit(EXIT_FAILURE);
	if (!(layer->type = (int*)ft_memalloc(sizeof(int) * WIDTH * HEIGHT)))
		exit(EXIT_FAILURE);
	if (!(layer->result = (t_pix*)ft_memalloc(sizeof(t_pix) * WIDTH * HEIGHT)))
		exit(EXIT_FAILURE);
}

static inline t_pix	interp_pix(t_pix b, t_pix f, float ratio)
{
	t_pix	new_pix;

	new_pix.c.a = 0;
	new_pix.c.b = (unsigned char)(ratio * (f.c.b - b.c.b) + b.c.b);
	new_pix.c.g = (unsigned char)(ratio * (f.c.g - b.c.g) + b.c.g);
	new_pix.c.r = (unsigned char)(ratio * (f.c.r - b.c.r) + b.c.r);
	return (new_pix);
}

static inline t_pix	get_pix(t_bmp *src, t_coord_f c_src)
{
	t_pix		corners[4];
	t_coord_i	c_src_i;
	t_coord_f	delta;
	int			i;

	c_src_i = (t_coord_i){(int)c_src.x, (int)c_src.y};
	i = src->dim.x * c_src_i.y + c_src_i.x;
	//return (src->pix[i]); // disable interpolation
	corners[0] = src->pix[i];
	corners[1] = src->pix[i + 1];
	i += src->dim.x;
	corners[2] = src->pix[i];
	corners[3] = src->pix[i + 1];
	delta = (t_coord_f){c_src.x - c_src_i.x, c_src.y - c_src_i.y};
	return (interp_pix(
			interp_pix(corners[0], corners[1], delta.x),
			interp_pix(corners[2], corners[3], delta.x),
			delta.y));
}

void				rendering_layer_render(t_rendering_layer *layer)
{
	int		i;
	float	fact;
	float	dist;
	t_pix	*result;

	result = layer->result;
	i = -1;
	while (++i < layer->n)
		result[i] = get_pix(&layer->bmp[layer->type[i]], layer->uv[i]);
	i = -1;
	while (++i < layer->n)
	{
		dist = layer->dist[i];
		if (dist > SHADOW_LIMIT)
		{
			fact = SHADOW_LIMIT / dist;
			result[i].c.b *= fact;
			result[i].c.g *= fact;
			result[i].c.r *= fact;
		}
	}
}

void				rendering_layer_render_sprite(t_rendering_layer *layer)
{
	int		i;
	float	fact;
	float	dist;
	t_pix	*result;

	result = layer->result;
	i = -1;
	while (++i < layer->n)
		result[i] = get_pix(&layer->bmp[layer->type[i]], layer->uv[i]);
	i = -1;
	while (++i < layer->n)
	{
		dist = layer->dist[i];
		if (result[i].i == 0xff00ff)
			result[i].c.a = 0xff;
		if (dist > SHADOW_LIMIT)
		{
			fact = SHADOW_LIMIT / dist;
			result[i].c.b *= fact;
			result[i].c.g *= fact;
			result[i].c.r *= fact;
		}
	}
}

void				rendering_layer_put(t_pix *pix, t_rendering_layer *layer)
{
	int			i;
	t_coord_i	*ij;
	t_pix		*p;

	ij = layer->ij;
	p = layer->result;
	i = -1;
	while (++i < layer->n)
	{
		pix[WIDTH * ij->y + ij->x] = *p;
		ij++;
		p++;
	}
}

void				rendering_layer_put_sprite(t_pix *pix, t_rendering_layer *layer)
{
	int			i;
	t_coord_i	*ij;
	t_pix		*p;

	ij = layer->ij;
	p = layer->result;
	i = -1;
	while (++i < layer->n)
	{
		pix[WIDTH * ij->y + ij->x].i = p->i * (p->c.a != 0xff) + pix[WIDTH * ij->y + ij->x].i * (p->c.a == 0xff);
		ij++;
		p++;
	}
}
